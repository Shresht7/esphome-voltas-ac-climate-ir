<#
.SYNOPSIS
    Merges multiple Pronto codes from log lines into a single string of hex codes.
.DESCRIPTION
    This script takes log lines containing Pronto codes, extracts the hex codes,
    and merges them into a single string separated by a delimiter (default: space)
.PARAMETER Logs
    The log lines containing the Pronto codes to be merged.
.EXAMPLE
    Get-Content pronto-codes.log | .\Merge-ProntoCodes.ps1
    This example reads Pronto codes from a log file and merges them into a single string of hex codes.
.EXAMPLE
    Get-Clipboard | .\Merge-ProntoCodes.ps1 > merged-codes.txt
    This example reads Pronto codes from the clipboard, merges them, and saves the result to a text file.
.EXAMPLE
    Get-Clipboard | .\Merge-ProntoCodes.ps1 | Set-Clipboard
    This example reads Pronto codes from the clipboard, merges them, and then copies the merged result back to the clipboard.
.NOTES
    Used to combine Pronto codes extracted from ESPHome logs into a single string for easier manipulation.
#>

[CmdletBinding()]
param (
    # The input string containing the log lines with Pronto codes to be merged
    [Parameter(Mandatory, ValueFromPipeline)]
    [string] $Logs,
    
    # Delimiter to separate the merged Pronto codes (default: space)
    [string] $Delimiter = " "
)

begin {
    $ProntoCodes = @()
}

process {
    # Extract the hex pronto codes from the log lines
    $Logs -split " " | ForEach-Object {
        if ($_ -match "([0-9A-Fa-f]{4})") {
            $ProntoCodes += $matches[1]
        }
    }
}

end {
    $ProntoCodes -join $Delimiter
}
