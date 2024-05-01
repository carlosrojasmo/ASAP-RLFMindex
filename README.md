README

### Para ejecutar el codigo luego de realizar un make se debe ejecutar:
### ./main nombre_alfabeto numero_opcion 
### Por ejemplo para ejecutar la opción 100 (ARRLFM_S18_RLMN(INT)s) para un alfabeto coreutils previamente cargados tanto los patrones como los archivos C, bwt, etc. se ejecutaría de la siguiente manera:
### ./main coreutils 100
### En caso de necesitar cambiar la Alternativa usada se debera entrar al archivo Arrlfm.cpp y cambiar la alternativa manualmente en la variable "flag" por la que se necesite siendo A1-> flag=1,  A2-> flag=2 ,A3-> flag=0 , A4-> flag=3 y recompilar usando make.