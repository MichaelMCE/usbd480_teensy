
Q:\Installs\Apps\ffmpeg\bin\ffmpeg.exe -i %1 -stats -f rawvideo -pix_fmt rgb565le -s 480x272 -vcodec rawvideo -frames 1000 "m:\RamDiskTemp\%~n1.raw"