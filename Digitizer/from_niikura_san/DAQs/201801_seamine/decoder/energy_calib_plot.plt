f(x)=a*x+b
fit f(x) "energy_calib_90Zr_sw.txt" u 2:1 via a,b
pl "energy_calib_90Zr_sw.txt" u 2:1
repl f(x)
