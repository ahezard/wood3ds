cp akmenu4/akmenu4_r4.nds akmenu4_r4_dsi.nds

python patch_ndsheader_dsiware.py --read akmenu4_r4_dsi.nds > akmenu4_r4.nds_header.txt

python patch_ndsheader_dsiware.py --mode dsi --title "AKMENU4" --maker 01 --code KAKE akmenu4_r4_dsi.nds

python patch_ndsheader_dsiware.py --read akmenu4_r4_dsi.nds > akmenu4_r4_dsi.nds_header_patched.txt

./make_cia --srl=akmenu4_r4_dsi.nds