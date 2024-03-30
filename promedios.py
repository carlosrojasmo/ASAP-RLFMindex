import os
import statistics


def equivalencias(entrada):
    equivalencias = {
    'ARRLFM_RLE_AP(RLMN)s': 'ASAP AP(RLMN) RLE',
    'ARRLFM_RLE_RLMN(AP)s': 'ASAP RLMN(AP) RLE',
    'ARRLFM_RLE_RLMN(INT)s': 'ASAP RLMN(INT) RLE',
    'ARRLFM_S18_AP(RLMN)s': 'ASAP AP(RLMN) S18',
    'ARRLFM_S18_RLMN(AP)s': 'ASAP RLMN(AP) S18',
    'ARRLFM_S18_RLMN(INT)s': 'ASAP RLMN(INT) S18',
    'ARRLFM_18_RL(4,GMR)s': 'ASAP FKKP(GMR) S18',
    'ARRLFM_18_RL(16,GMR)s': 'ASAP FKKP(GMR) S18',
    'ARRLFM_18_RL(32,GMR)s': 'ASAP FKKP(GMR) S18',
    'ARRLFM_RE_RL(4,GMR)s': 'ASAP FKKP(GMR) RLE',
    'ARRLFM_RE_RL(16,GMR)s': 'ASAP FKKP(GMR) RLE',
    'ARRLFM_RE_RL(32,GMR)s': 'ASAP FKKP(GMR) RLE',
    'ARRLFM_18_RL(4,AP)s': 'ASAP FKKP(AP) S18',
    'ARRLFM_18_RL(16,AP)s': 'ASAP FKKP(AP) S18',
    'ARRLFM_18_RL(32,AP)s': 'ASAP FKKP(AP) S18',
    'ARRLFM_RE_RL(4,AP)s': 'ASAP FKKP(AP) RLE',
    'ARRLFM_RE_RL(16,AP)s': 'ASAP FKKP(AP) RLE',
    'ARRLFM_RE_RL(32,AP)s': 'ASAP FKKP(AP) RLE'}
    return equivalencias[entrada]

def cantidadPalabras(alfabeto):
    if alfabeto == 'coreutils':
        return 93319681
    elif alfabeto == 'einstein.de.txt':
        return 29970946
    elif alfabeto == 'einstein.en.txt':
        return 159119879
    elif alfabeto == 'english.001.2':
        return 39894587
    elif alfabeto == 'world_leaders':
        return 31203286
def calcular_promedio_tiempo(archivo):
    tiempos = []
    with open(archivo, 'r') as f:
        for linea in f:
            _, tiempo = linea.strip().split(' ')
            tiempos.append(float(tiempo))
    return statistics.mean(tiempos)

def generar_archivo_promedio(archivo,memoria,texto,path,alternativa):
    promedio = calcular_promedio_tiempo(archivo)
    m=os.path.splitext(archivo)[0].split("_")[-1]
    alternativa="_".join(alternativa.split("_")[0:-1])
    alternativa=equivalencias(alternativa)
    nombre_archivo = path+"\\"+texto+"-"+m+"-"+alternativa
    cantidadPalabras=cantidadPalabras(texto)
    with open(nombre_archivo, 'w') as f:
        f.write(str(promedio)+" "+(memoria*8)/cantidadPalabras)

def procesar_carpeta(directorio):
    for texto in os.listdir(directorio):
        directorio2=os.path.join(directorio, texto)
        if os.path.isdir(directorio2):
            for carpeta1 in os.listdir(directorio2):
                directorio1=os.path.join(directorio2, carpeta1)
                if os.path.isdir(directorio1):
                    for carpeta in os.listdir(directorio1):
                        if (carpeta1=="Memoria"):
                            break
                        carpeta_path = os.path.join(directorio1, carpeta)
                        if os.path.isdir(carpeta_path):
                            for archivo in os.listdir(carpeta_path):
                                if (carpeta=="ResultadosTar"):
                                    break
                                archivo_path = os.path.join(carpeta_path, archivo)
                                
                                if (os.path.isfile(archivo_path) and  (archivo_path.strip().split("_")[-1].isdigit())):
                                    carpeta_memoria=os.path.join(directorio2, "Memoria"+"/"+carpeta+"_Memoria.txt")
                                    with open(carpeta_memoria, 'r') as f:
                                        memoria=f.read()
                                    generar_archivo_promedio(archivo_path,memoria,texto,carpeta_path,archivo)

directorio_base = './'  # Cambia esto por la ruta de tu directorio
procesar_carpeta(directorio_base)
