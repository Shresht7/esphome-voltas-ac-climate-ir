param(
    # Path to the directory containing the Pronto codes
    [ValidateScript({ Test-Path $_ -PathType Container })]
    [string] $Directory = "$PSScriptRoot\..\protocol\pronto-code",

    # Output file name for the generated JSON
    [string] $OutputFileName = "raw-pronto-codes.json"
) 

$JSON = @()

Get-ChildItem -Path $Directory -Filter "*.hex" | ForEach-Object {
    $FileName = $_.BaseName
    $Content = Get-Content -Path $_.FullName -Raw
    $JSON += @{
        label = $FileName
        hex  = $Content
    }
}

$OutputPath = Join-Path -Path $Directory -ChildPath $OutputFileName
$JSON | ConvertTo-Json -Depth 3 | Set-Content -Path $OutputPath -Encoding UTF8

Write-Host "Pronto codes JSON file generated at: $OutputPath"
