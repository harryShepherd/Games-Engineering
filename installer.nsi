!define APPNAME "CubeZone"
!define VERSION "1.0.0"

Name "${APPNAME}"
OutFile "${APPNAME}Installer.exe"

InstallDir "$PROGRAMFILES\${APPNAME}"

ShowInstDetails show
ShowUninstDetails show

Section "Install CubeZone" SecInstall

    ; Create install directory
    SetOutPath "$INSTDIR"

    ; Copy main executable
    File "${APPNAME}.exe"

    ; Copy all files in bin directory
    File "*.*"

    ; Copy resources folder
    SetOutPath "$INSTDIR\resources"
    File /r "resources\*.*"

    ; Start Menu shortcut
    SetOutPath "$INSTDIR"
    
    CreateDirectory "$SMPROGRAMS\${APPNAME}"
    CreateShortcut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe"

    ; Desktop shortcut
    CreateShortcut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe"

    CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall${APPNAME}.lnk" $INSTDIR\Uninstall${APPNAME}.exe"
    WriteUninstaller "$INSTDIR\Uninstall${APPNAME}.exe"

SectionEnd

; ------------------------------------------
; Uninstall Section
; ------------------------------------------

Section "Uninstall"

    ; Remove shortcuts
    Delete "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk"
    RMDir "$SMPROGRAMS\${APPNAME}"
    Delete "$DESKTOP\${APPNAME}.lnk"

    ; Remove installed files
    Delete "$INSTDIR\${APPNAME}.exe"
    Delete "$INSTDIR\*.*"

    ; Remove resources folder
    RMDir /r "$INSTDIR\resources"

    ; Remove installation directory
    RMDir "$INSTDIR"

SectionEnd