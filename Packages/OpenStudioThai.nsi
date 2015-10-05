; Script generated by the NSIS Studio v1.3.4565.19537

!include "LogicLib.nsh"
!include "WinVer.nsh"
!include "FileAssociation.nsh"

; Define helper variables
!define PRODUCT_NAME "OpenStudioThai"
!define PRODUCT_VERSION "1.7.0.0"
!define PRODUCT_DISPLAY_NAME "${PRODUCT_NAME} v${PRODUCT_VERSION}"
!define PRODUCT_PUBLISHER "ToBeOne Technology Co, Ltd"
!define PRODUCT_WEB_SITE "ww.2b1tech.com"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; Define user variables
var ISEXSIT_DOTNET_FULL_INTSTALLED
;var ISEXSIT_DOTNET_CLNT_INTSTALLED

SetCompress off
;SetCompress  auto

; Interface Settings
!define MULTIUSER_EXECUTIONLEVEL Admin
!define MULTIUSER_NOUNINSTALL
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!include "MultiUser.nsh"
!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "D:\OpenStudio\openstudiocore\icons\os.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "D:\OpenStudio\openstudiocore\icons\nsis.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "D:\OpenStudio\openstudiocore\icons\nsis.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"
!define MUI_COMPONENTSPAGE_CHECKBITMAP "${NSISDIR}\Contrib\Graphics\Checks\colorful.bmp"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Page Settings
; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Start menu page
var ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Thai"

; MUI end

LangString Title ${LANG_THAI} "���й�"
LangString Title ${LANG_ENGLISH} "Suggestion"

LangString Message ${LANG_THAI} "�����������ö��ҹ����� Open Studion Thai Edition  1.7.0 ��� Energy Plus 8.2.0  �����дǡ�Ѻ�ҹ�͡Ẻ ���й����Դ������������㹡���͡Ẻ SketchUp Make 2015 ��� �������Ҿ�����ѹ� Package Installer ���"
LangString Message ${LANG_ENGLISH} "To be able to use Open Studion Thai Edition 1.7.0 and Energy Plus 8.2.0 Easily design. It is recommended to install SketchUp Make 2015 that is included in the Package Installer."


Name "${PRODUCT_DISPLAY_NAME}"
OutFile "D:\OpenStudio\Packages\OpenStudioThai.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
BrandingText "${PRODUCT_PUBLISHER}"
SetOverwrite on
Caption "${PRODUCT_DISPLAY_NAME} Setup"

; Installer file metadata
VIProductVersion "1.7.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "OpenStudioThai"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "1.7.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "ToBeOne Technology Co, Ltd"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "? ${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_NAME} Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.0.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" ""

Section "SketchUpMake" SEC001
  SetOutPath "$INSTDIR"
  SetOutPath "$APPDATA"
  File "D:\OpenStudio\Packages\SketchUp2015-x86.msi"
  File "D:\OpenStudio\Packages\dotNetFx40_Full_x86_x64.exe"
  ;File "D:\OpenStudio\Packages\dotNetFx40_Client_x86_x64.exe"
  
  ${If} $ISEXSIT_DOTNET_FULL_INTSTALLED == "false"
  SetOutPath "$APPDATA"
  ExecWait '"$APPDATA\dotNetFx40_Full_x86_x64.exe"'
  ${EndIf}
  
  ;${If} $ISEXSIT_DOTNET_CLNT_INTSTALLED == "false"
  ;SetOutPath "$APPDATA"
  ;ExecWait '"$APPDATA\dotNetFx40_Client_x86_x64.exe"'
  ;${EndIf}
  
  SetOutPath "$APPDATA"
  ExecWait '"msiexec" /i "$APPDATA\SketchUp2015-x86.msi"'
SectionEnd

Section "EnergyPlus" SEC002
  SetOutPath "$INSTDIR"
  SetOutPath "$APPDATA"
  File "F:\Release Pakages\EnergyPlus-8.2.0-8397c2e30b-Windows-i386.exe"
  SetOutPath "$APPDATA"
  ExecWait '"$APPDATA\EnergyPlus-8.2.0-8397c2e30b-Windows-i386.exe"'
SectionEnd

Section "OpenStudio" SEC003
  SetOutPath "$INSTDIR"
  SetOutPath "$APPDATA"
  File "D:\OpenStudio\build\_CPack_Packages\win32\NSIS\OpenStudio-1.7.0.e40169d9a1-Windows.exe"
  SetOutPath "$APPDATA"
  ExecWait '"$APPDATA\OpenStudio-1.7.0.e40169d9a1-Windows.exe"'
SectionEnd


;Set user variables
Function .onSelChange
SectionGetFlags ${SEC001} $R0
IntOp $R0 $R0 & ${SF_SELECTED}

${If} $R0 != ${SF_SELECTED}
    System::Call 'user32::MessageBox(i $HWNDPARENT, t "$(message)",t "$(Title)", i 64) v r0'
${EndIf}
FunctionEnd

Function .onInit
  !insertmacro MULTIUSER_INIT
  !define MUI_LANGDLL_ALWAYSSHOW
  !insertmacro MUI_LANGDLL_DISPLAY

  ;Set user variables
  ReadRegStr $ISEXSIT_DOTNET_FULL_INTSTALLED "SHCTX" "SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\full" "Install"
  ${If} $ISEXSIT_DOTNET_FULL_INTSTALLED == ""
    StrCpy $ISEXSIT_DOTNET_FULL_INTSTALLED "false"
  ${Else}
    StrCpy $ISEXSIT_DOTNET_FULL_INTSTALLED "true"
  ${EndIf}
  
  ;ReadRegStr $ISEXSIT_DOTNET_CLNT_INTSTALLED "SHCTX" "SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Client" "install"
  ;${If} $ISEXSIT_DOTNET_CLNT_INTSTALLED == ""
  ;  StrCpy $ISEXSIT_DOTNET_CLNT_INTSTALLED "false"
  ;${Else}
  ;  StrCpy $ISEXSIT_DOTNET_CLNT_INTSTALLED "true"
  ;${EndIf}

  
  ;Set section flags
  StrCpy $0 0
  IntOp $0 $0 | ${SF_SELECTED}
  SectionSetFlags ${SEC001} $0
  
  StrCpy $0 0
  IntOp $0 $0 | ${SF_SELECTED}
  IntOp $0 $0 | ${SF_RO}
  SectionSetFlags ${SEC002} $0
  
  StrCpy $0 0
  IntOp $0 $0 | ${SF_SELECTED}
  IntOp $0 $0 | ${SF_RO}
  SectionSetFlags ${SEC003} $0
FunctionEnd

Section -AdditionalIcons
  StrCpy $ICONS_GROUP "${PRODUCT_NAME}"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  WriteIniStr "$SMPROGRAMS\$ICONS_GROUP\Website.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\${PRODUCT_NAME}.lnk" "$EXEDIR"
SectionEnd

Section -Post
	Delete '"$APPDATA\dotNetFx40_Full_x86_x64.exe"'
	;Delete '"$APPDATA\dotNetFx40_Client_x86_x64.exe"'
	Delete '"$APPDATA\SketchUp2015-x86.msi"'
	Delete '"$APPDATA\EnergyPlus-8.2.0-8397c2e30b-Windows-i386.exe"'
	Delete '"$APPDATA\OpenStudio-1.7.0.e40169d9a1-Windows.exe"'
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC001} "Install SketcUpMake 32 bit"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC002} "Install EnergyPlus-8.2.0-8397c2e30b 32 bit"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC003} "Install OpenStudio 1.7.0 32 bit"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

