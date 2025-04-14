param (
    [string][Parameter(Mandatory=$true)]$executablePath,
    [switch][Alias('f')]$force = $false
)

$isPathValid = Test-Path -PathType Leaf $executablePath
if(-not $isPathValid) {
    Write-Host 'Path invalid'
    exit
}

$fileHeader = file $executablePath;
$isExecutable = $fileHeader -like "*ELF 32-bit*";
if(-not $isExecutable) {
    if(-not $force) {
        Write-Host 'The file is not an executable'
        exit
    }
    else {
        Write-Host 'The file is not an executable'
    }
}

gdb -x ./files/gdb_startup $executablePath
