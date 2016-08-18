cp akmenu4/akmenu4_r4.nds akmenu4_r4_dsi.nds
cp akmenu4/akmenu4_r4.nds akmenu4_r4_dsi_nogba.nds
cp akmenu4/akmenu4_r4.nds akmenu4_r4_ds.nds

python patch_ndsheader_dsiware.py --read akmenu4_r4_dsi.nds > akmenu4_r4.nds_header.txt

./dlditool.exe dldi/r4_sd/r4_sd.dldi akmenu4_r4_dsi.nds

python patch_ndsheader_dsiware.py --mode dsi --title "AKMENU4" --maker 01 --code KAKE akmenu4_r4_dsi.nds

python patch_ndsheader_dsiware.py --mode ds --title "AKMENU4" --maker 01 --code AFFE akmenu4_r4_ds.nds

python patch_ndsheader_dsiware.py --read akmenu4_r4_dsi.nds > akmenu4_r4_dsi.nds_header_patched.txt

python patch_ndsheader_dsiware.py --mode dsinogba --title "AKMENU4" --maker 01 --code KAKE akmenu4_r4_dsi_nogba.nds

./make_cia --srl=akmenu4_r4_dsi.nds

./make_cia --srl=akmenu4_r4_ds.nds