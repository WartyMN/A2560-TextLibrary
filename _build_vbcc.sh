#!/bin/zsh

export VBCC=/opt/vbcc
export PATH=$VBCC/bin:$PATH

cd /Users/micahbly/dev/bbedit-workspace-a2560/lib_text

#vc +/opt/vbcc/config/a2560-s28 -o text.s28 main.c general.c general_a2560.c lib_text.c lib_text_test.c
#vc +/opt/vbcc/config/a2560-s28 -o text.s28 test3.c general.c general_a2560.c

#vc +/opt/vbcc/config/a2560-s28-micah -o text.s28 main.c general.c general_a2560.c lib_text.c lib_text_test.c
#vc +/opt/vbcc/config/a2560-s28-micah -o text.s28 main.c general.c general_a2560.c lib_text.c

vc +/opt/vbcc/config/a2560-s28-micah -o text.s28 main.c lib_general.c lib_general_a2560.c lib_text.c
#vc +/opt/vbcc/config/a2560-s28 -o text.s28 main.c lib_general.c lib_general_a2560.c lib_text.c

# no tests
#vc +/opt/vbcc/config/aos68k -o build_vbcc/wb2k about_window.c app.c dict.c dict_entry.c file.c file_mover.c folder.c file_type.c file_type_db.c general.c general_amiga.c icon.c info_file.c info_panel.c label.c list.c list_panel.c menu.c mouse.c localize.c prefs.c status_window.c window.c -g -lauto -lamiga -prof -lmieee -stack-check

# with tests
#vc +/opt/vbcc/config/aos68k -o build_vbcc/wb2k about_window.c app.c dict.c dict_entry.c file.c file_mover.c folder.c file_type.c file_type_db.c general.c general_amiga.c icon.c info_file.c info_panel.c label.c list.c list_panel.c menu.c mouse.c localize.c prefs.c status_window.c window.c dict_test.c file_type_db_test.c icon_test.c prefs_test.c -g -lauto -lamiga -prof -lmieee -stack-check

# make binary with srec. eg
/usr/local/Homebrew/bin/srec_cat text.s28 -Motorola -address-length=3 -o text.bin -binary

# delete s28, no longer need it
rm text.s28

# convert binary to intel. eg
#/usr/local/Homebrew/bin/srec_cat hello.bin -binary -offset 0x20000 -o hello.hex -intel
/usr/local/Homebrew/bin/srec_cat text.bin -binary -o text.hex -intel

# delete binary, no longer need it
rm text.bin

# move hex to morfe folder for processing with BBedit and running with morfe
mv text.hex ../morfe/

# SEMI_MANUAL WAY
# Run BBEdit Text Factory "clean_intel_hex.textfactory". it will remove the empty junk and leave correct addressing. Time will tell how accurate/flexible the regex in there is though, so don't convert that to shell/perl until assessed with more source compiles. After the text factory is run, the .bin.hex file can be loaded into morfe. 

echo "VBCC build script complete\n"
