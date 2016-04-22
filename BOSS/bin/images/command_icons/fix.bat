for %%f in (*.png) DO ( convert "%%f" -strip -resize 32x32! "%%f" )
