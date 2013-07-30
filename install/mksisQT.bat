@echo off

makesis mreader_0x20040215.pkg mreader.sis
signsis mreader.sis mreader.sisx rd.cer rd-key.pem

del mreader.sis

echo .
echo All steps finished...