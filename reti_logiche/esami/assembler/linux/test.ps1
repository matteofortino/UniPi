param(
    [string][Parameter(Mandatory=$true)]$executable,
    [string][Parameter(Mandatory=$true)]$inputFile,
    [string]$outputFile = $null,
    [switch][Alias('f')]$force = $false
)

$isPathValid = Test-Path -PathType Leaf $executable
if(-not $isPathValid) {
    Write-Host 'Path invalid: executable'
    exit
}

$isPathValid = Test-Path -PathType Leaf $inputFile
if(-not $isPathValid) {
    Write-Host 'Path invalid: input'
    exit
}

$fileHeader = file $executable;
$isExecutable = $fileHeader -like "*ELF 32-bit*";
if(-not $isExecutable) {
    Write-Host 'The file is not an executable'
    if(-not $force) {
        exit
    }
}

# Powershell is 'too smart' about newlines, which is annoying for us
# So we run it in bash instead
if([string]::IsNullOrEmpty($outputFile)) {
    $command = "'$executable' < '$inputFile'";
    $command | bash
} else {
    $command = "'$executable' < '$inputFile' > '$outputFile' 2>&1";
    $command | bash
}