; -*- no -*-
; $Id$
;
; -- Inno Setup installer script for PuTTY and its related tools.
;
; TODO for future releases:
;
;  - It might be nice to have an option to add PSCP, Plink and PSFTP to
;    the PATH. See wish `installer-addpath'.
;
;  - Maybe a "custom" installation might be useful? Hassle with
;    UninstallDisplayIcon, though.

[Setup]
AppName={cm:MyAppName}
AppVerName={cm:MyAppVerName}
;VersionInfoDescription={cm:MyVersionDescription}
VersionInfoTextVersion={cm:MyVersionInfo}
AppVersion=0.59
VersionInfoVersion=0.59.0.0
AppPublisher=Yebisuya Hompo
AppPublisherURL=http://yebisuya.dip.jp/Software/PuTTY/
AppReadmeFile={app}\readme-putty-0.59-JP_Y.html
DefaultDirName={code:DefaultBaseDir}\PuTTY
DefaultGroupName={cm:MyAppName}
SetupIconFile=installer.ico
UninstallDisplayIcon={app}\putty.exe
ChangesAssociations=yes
;ChangesEnvironment=yes -- when PATH munging is sorted (probably)
Compression=zip/9
AllowNoIcons=yes
AppId=PuTTY
PrivilegesRequired=none
OutputBaseFilename=putty-0.59-JP_Y-setup

[Languages]
Name: "eng"; MessagesFile: "Default.isl,compiler:Default.isl"
Name: "jpn"; MessagesFile: "Japanese.isl,compiler:Languages\Japanese.isl"

[Components]
Name: PuTTY; Description: "{cm:MyAppName}({cm:PuTTYComment})"; Types: Full Custom Compact; Flags: fixed
Name: pfwd; Description: "{cm:PfwdComponentDescription}"; Types: Full
Name: plinkw; Description: "{cm:PlinkwComponentDescription}"; Types: Full
Name: jpnlng; Description: "{cm:JpnLngComponentDescription}"; Types: Full; Languages: jpn
Name: source; Description: "{cm:SourceComponentDescription}"; Types: Full

[Files]
; We flag all files with "restartreplace" et al primarily for the benefit
; of unattended un/installations/upgrades, when the user is running one
; of the apps at a time. Without it, the operation will fail noisily in
; this situation.
; This does mean that the user will be prompted to restart their machine
; if any of the files _were_ open during installation (or, if /VERYSILENT
; is used, the machine will be restarted automatically!). The /NORESTART
; flag avoids this.
; It might be nicer to have a "no worries, replace the file next time you
; reboot" option, but the developers have no interest in adding one.
; NB: apparently, using long (non-8.3) filenames with restartreplace is a
; bad idea. (Not that we do.)
Source: "putty.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion restartreplace uninsrestartdelete
Source: "pageant.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion restartreplace uninsrestartdelete
Source: "puttygen.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion restartreplace uninsrestartdelete
Source: "pscp.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion restartreplace uninsrestartdelete
Source: "psftp.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion restartreplace uninsrestartdelete
Source: "plink.exe"; DestDir: "{app}"; Flags: promptifolder replacesameversion restartreplace uninsrestartdelete
Source: "website.url"; DestDir: "{app}"; Flags: restartreplace uninsrestartdelete
Source: "..\doc\putty.chm"; DestDir: "{app}"; Flags: restartreplace uninsrestartdelete
Source: "..\doc\putty.hlp"; DestDir: "{app}"; Flags: restartreplace uninsrestartdelete
Source: "..\doc\putty.cnt"; DestDir: "{app}"; Flags: restartreplace uninsrestartdelete
Source: "..\LICENCE"; DestDir: "{app}"; Flags: restartreplace uninsrestartdelete
Source: "README.txt"; DestDir: "{app}"; Flags: restartreplace uninsrestartdelete
Source: "plinkw.exe"; DestDir: "{app}"; Components: plinkw
Source: "pfwd.exe"; DestDir: "{app}"; Components: pfwd
Source: "pfwd_sample.ini"; DestDir: "{app}"; Components: pfwd
Source: "pageant.lng"; DestDir: "{app}"; Components: jpnlng
Source: "pfwd.lng"; DestDir: "{app}"; Components: jpnlng and pfwd
Source: "plinkw.lng"; DestDir: "{app}"; Components: jpnlng and plinkw
Source: "putty.lng"; DestDir: "{app}"; Components: jpnlng
Source: "puttygen.lng"; DestDir: "{app}"; Components: jpnlng
Source: "puttytel.lng"; DestDir: "{app}"; Components: jpnlng
Source: "..\..\putty-doc\readme-putty-0.59-JP_Y.html"; DestDir: "{app}"; Flags: isreadme
Source: "..\..\putty-doc\putty-0.59-JP_Y.patch"; DestDir: "{app}"; Components: source
Source: "puttyd.ico"; DestDir: "{app}"; Components: source

[Icons]
Name: "{group}\{cm:PuTTYShortcutName}"; Filename: "{app}\putty.exe"
; We have to fall back from the .chm to the older .hlp file on some Windows
; versions.
Name: "{group}\{cm:PuTTYManualShortcutName}"; Filename: "{app}\putty.chm"; MinVersion: 4.1,5.0
Name: "{group}\{cm:PuTTYManualShortcutName}"; Filename: "{app}\putty.hlp"; OnlyBelowVersion: 4.1,5.0
Name: "{group}\{cm:PuTTYWebSiteShortcutName}"; Filename: "{app}\website.url"
Name: "{group}\{cm:PSFTPShortcutName}"; Filename: "{app}\psftp.exe"
Name: "{group}\{cm:PuTTYgenShortcutName}"; Filename: "{app}\puttygen.exe"
Name: "{group}\{cm:PageantShortcutName}"; Filename: "{app}\pageant.exe"
Name: "{commondesktop}\{cm:PuTTYShortcutName}"; Filename: "{app}\putty.exe"; Tasks: desktopicon and common
Name: "{userdesktop}\{cm:PuTTYShortcutName}"; Filename: "{app}\putty.exe"; Tasks: desktopicon and not common
; Putting this in {commonappdata} doesn't seem to work, on 98SE at least.
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{cm:PuTTYShortcutName}"; Filename: "{app}\putty.exe"; Tasks: quicklaunchicon

[Tasks]
Name: desktopicon; Description: "{cm:desktopiconDescription}"; GroupDescription: "{cm:AdditionalIconsGroupDescription}"; Flags: unchecked
Name: quicklaunchicon; Description: "{cm:quicklaunchiconDescription}"; GroupDescription: "{cm:AdditionalIconsGroupDescription}"; Flags: unchecked
Name: associate; Description: "{cm:associateDescription}"; GroupDescription: "{cm:OtherTasksGroupDescription}"
Name: saveini; Description: "{cm:SaveiniDescription}"; GroupDescription: "{cm:OtherTasksGroupDescription}"; Flags: unchecked
Name: common; Description: "{cm:desktopicon_commonDescription}"; GroupDescription: "{cm:OtherTasksGroupDescription}"; Flags: unchecked

[Registry]
Root: HKCR; Subkey: ".ppk"; ValueType: string; ValueName: ""; ValueData: "PuTTYPrivateKey"; Flags: uninsdeletevalue; Tasks: associate and common
Root: HKCR; Subkey: "PuTTYPrivateKey"; ValueType: string; ValueName: ""; ValueData: "{cm:PPKTypeDescription}"; Flags: uninsdeletekey; Tasks: associate and common
Root: HKCR; Subkey: "PuTTYPrivateKey\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\pageant.exe,0"; Tasks: associate and common
Root: HKCR; Subkey: "PuTTYPrivateKey\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\pageant.exe"" ""%1"""; Tasks: associate and common
Root: HKCR; Subkey: "PuTTYPrivateKey\shell\edit"; ValueType: string; ValueName: ""; ValueData: "{cm:PPKEditLabel}"; Tasks: associate and common
Root: HKCR; Subkey: "PuTTYPrivateKey\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\puttygen.exe"" ""%1"""; Tasks: associate and common
Root: HKCU; Subkey: "Software\Classes\.ppk"; ValueType: string; ValueName: ""; ValueData: "PuTTYPrivateKey"; Flags: uninsdeletevalue; Tasks: associate and not common
Root: HKCU; Subkey: "Software\Classes\PuTTYPrivateKey"; ValueType: string; ValueName: ""; ValueData: "{cm:PPKTypeDescription}"; Flags: uninsdeletekey; Tasks: associate and not common
Root: HKCU; Subkey: "Software\Classes\PuTTYPrivateKey\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\pageant.exe,0"; Tasks: associate and not common
Root: HKCU; Subkey: "Software\Classes\PuTTYPrivateKey\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\pageant.exe"" ""%1"""; Tasks: associate and not common
Root: HKCU; Subkey: "Software\Classes\PuTTYPrivateKey\shell\edit"; ValueType: string; ValueName: ""; ValueData: "{cm:PPKEditLabel}"; Tasks: associate and not common
Root: HKCU; Subkey: "Software\Classes\PuTTYPrivateKey\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\puttygen.exe"" ""%1"""; Tasks: associate and not common
; Add to PATH on NT-class OS?

[INI]
Filename: "{app}\putty.ini"; Section: "Generic"; Key: "UseIniFile"; Tasks: saveini and common
Filename: "{userappdata}\PuTTY\putty.ini"; Section: "Generic"; Key: "UseIniFile"; Tasks: saveini and not common

[UninstallRun]
; -cleanup-during-uninstall is an undocumented option that tailors the
; message displayed.
; XXX: it would be nice if this task weren't run if a silent uninstall is
;      requested, but "skipifsilent" is disallowed.
Filename: "{app}\putty.exe"; Parameters: "-cleanup-during-uninstall"; RunOnceId: "PuTTYCleanup"; StatusMsg: "{cm:UninstallStatusMsg}"

[Code]
function DefaultBaseDir(Param: String) : String;
begin
    if IsAdminLoggedOn then
      Result := ExpandConstant('{pf}')
    else
      Result := ExpandConstant('{userappdata}');
end;
