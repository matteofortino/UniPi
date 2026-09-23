use JSON::XS;
use Data::Dumper;
use IO::Handle;

($dev = $0) =~ s/\.pl$//;

open $in, "<", "$dev.out";
open $out, ">", "$dev.in";
open $async, ">", "${dev}a.in";

$out->autoflush(1);
$async->autoflush(1);

$json = JSON::XS->new;

$config = {
		bars => [
			{
				name => 'ce-io',
				type => 'io',
				size => 16,
			}
		],
		'interrupt-pin' => 2
	};


my $CTL;
my $STS = 0;
my $RBR = 0;
my @values = ();
my $pend_intr;
#$debug = 1;

sub dbg {
	my $arg = shift;
	if ($debug) {
		print($arg);
	}
}

sub send {
	my ($ch, $msg) = @_;
	my $r = $json->encode($msg);
	&dbg("### $0: sending '$r' on $ch\n");
	if ($ch eq "sync") {
		print $out "$r\n";
	} else {
		print $async "$r\n";
	}
}

sub receive {
	while (<$in>) {
	       	if ($obj = $json->incr_parse($_)) {
			&dbg(Dumper($obj));
			return $obj;
		}
	}
	exit;
}


sub send_ret {
	my $arg = shift || 0;
	&send("sync", { return => int($arg)});
}

sub send_intr {
	return unless ($CTL & 1);

	if (($STS & 1) && !$pend_intr) {
		&send("async", { "raise-irq" => 1 });
		$pend_intr = 1;
	}
}

sub clear_intr {
	if ($pend_intr) {
		&send("async", { "lower-irq" => 1 });
		$pend_intr = 0;
	}
}

sub new_value {
	if (!@values) {
		&dbg("### no values\n");
		return 0;
	}
	$RBR = shift @values;
	&dbg("### new value: $RBR\n");
	return 1;
}

while (1) {
	$obj = &receive;
	if ($$obj{'get-config'}) {
		&send("sync", $config);
		&send_ret;
	} elsif ($i = $$obj{'write'}) {
		$a = $i->{'addr'};
		$s = $i->{'size'};
		$v = $i->{'val'};
		&dbg("### $0: writing $v at $a, size $s\n");
		&send_ret if  ($s != 1);
		if ($a == 0) {
			($old, $CTL) = ($CTL, $v & 1);
			&dbg("### CTL $old -> $CTL\n");
			if ($old ^ $v) {
				# we are changing the interrupt enabled bit
				if ($old) {
					# was enabled, clear any pending intr
					&clear_intr
				} else {
					# was disabled, send intr if necessary
					&send_intr
				}
			}
		} elsif ($a == 12) {
			# debug register
			push @values, $v;
			if (! ($STS & 1)) {
				&new_value;
				$STS |= 1;
				&send_intr;
			}
		}
		&send_ret;
	} elsif ($i = $$obj{'read'}) {
		$a = $i->{'addr'};
		&send_ret if  ($a % 4 || $a >= 12);
		$s = $i->{'size'};
		&send_ret if  ($s != 1);
		&dbg("### $0: reading from $a\n");
		if ($a == 0) {
			&send_ret($CTL);
		} elsif ($a == 4) {
			&send_ret($STS);
		} elsif ($a == 8) {
			&clear_intr;
			&send_ret($RBR);
			$STS &= ~1;
			if (&new_value) {
				$STS |= 1;
				&send_intr;
			}
		} else {
			&send_ret(0);
		}
	}			
}
