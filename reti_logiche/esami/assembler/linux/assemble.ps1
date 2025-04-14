param (
    [string][Parameter(Mandatory=$true)]$sourcePath,
    [switch][Alias('f')]$force = $false
)

$isPathValid = Test-Path -PathType Leaf $sourcePath
if(-not $isPathValid) {
    Write-Host 'Path invalid';
    exit
}

$fileHeader = file $sourcePath;
$isExecutable = $fileHeader -like "*ELF 32-bit*";
if($isExecutable) {
    if(-not $force) {
        Write-Host 'The file is an executable';
        exit
    }
    else {
        Write-Host 'The file is an executable';
    }
}

$sourceFile = Get-ChildItem $sourcePath;

$dir = $sourceFile.Directory;
$name = $sourceFile.BaseName;

gcc -m32 -o "$dir/$name" '-Wa,-a' '-Wa,--defsym,LINUX=1' > "$dir/$name.lst" -g ./files/main.c $sourceFile
if($LASTEXITCODE -ne 0) {
    Write-Host "Assembler error"
}