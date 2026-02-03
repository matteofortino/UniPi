use JSON::XS;
use Data::Dumper;
use IO::Handle;
use Time::HiRes qw(ualarm);

($dev = $0) =~ s/\.pl$//;

open $in, "<", "$dev.out";
open $out, ">", "$dev.in";
open $ain, "<", "${dev}a.out";
open $aout, ">", "${dev}a.in";

$out->autoflush(1);
$aout->autoflush(1);

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


my $CTL = 0;
my $STS = 1;
my $RBR = 65;
my $pend_intr;

sub dbg {
	print @_ if $debug;
}

sub send {
	my ($ch, $msg) = @_;
	my $r = $json->encode($msg);
	&dbg("### $0: sending '$r' on $ch\n");
	if ($ch eq "sync") {
		print $out "$r\n";
	} else {
		print $aout "$r\n";
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

sub areceive {
	while (<$ain>) {
	       	if ($obj = $json->incr_parse($_)) {
			&dbg(Dumper($obj));
			return $obj;
		}
	}
	exit;
}

sub send_ret {
	my $arg = shift || 0;
	&send("sync", { return => $arg});
}

sub send_intr {
	return unless ($CTL & 1);

	if (($STS & 1) && !$pend_intr) {
		#&send("async", { "raise-irq" => 1 });
		#&areceive;
		$pend_intr = 1;
		&ualarm(10000);
	}
}

sub clear_intr {
	if ($pend_intr) {
		&send("sync", { "lower-irq" => 1 });
		&receive;
		$pend_intr = 0;
	}
}

sub new_value {
	if (!$pend_intr) {
		$RBR++;
		if ($RBR > 90) {
			$RBR = 65;
		}
		$STS |= 1;
		&send_intr;
	}
}

$SIG{ALRM} = sub { 
	&send("async", { "raise-irq" => 1 });
	&areceive;
};

while (1) {
	&dbg("### $0: waiting for command\n");
	$obj = &receive;
	if ($$obj{'get-config'}) {
		&send("sync", $config);
		&send_ret;
	} elsif ($i = $$obj{'write'}) {
		$a = int($i->{'addr'});
		&send_ret if  ($a != 0);
		$s = int($i->{'size'});
		$v = int($i->{'val'});
		&send_ret if  ($s != 1);
		&dbg("### $0: writing $v at $a\n");
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
		&send_ret;
	} elsif ($i = $$obj{'read'}) {
		$a = $i->{'addr'};
		&send_ret if  ($a % 4 || $a >= 12);
		$s = $i->{'size'};
		&send_ret if  ($s != 1);
		&dbg("### $0: reading from $a\n");
		if ($a == 0) {
			&send_ret(int($CTL));
		} elsif ($a == 4) {
			&send_ret(int($STS));
		} else {
			&clear_intr;
			&send_ret(int($RBR));
			$STS &= ~1;
			&new_value;
		}
	}			
}
