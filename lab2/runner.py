import subprocess
import os
import time
import concurrent.futures
import threading
from queue import Queue

# Lock global para la escritura sincronizada al archivo
file_lock = threading.Lock()
# Cola para almacenar los resultados
results_queue = Queue()

def run_program_for_file(program_path, input_file):
    """
    Ejecuta el programa con un archivo de entrada específico y retorna el resultado
    """
    thread_id = threading.current_thread().name
    print(f"\nDebug [{thread_id}] - Iniciando procesamiento para archivo: {input_file}")
    
    try:
        # Crear el proceso
        print(f"Debug [{thread_id}] - Iniciando el proceso...")
        process = subprocess.Popen(
            [program_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        # Secuencia de interacción con el menú
        print(f"Debug [{thread_id}] - Enviando '1' para Read file")
        process.stdin.write("1\n")
        process.stdin.flush()
        time.sleep(0.5)
        
        print(f"Debug [{thread_id}] - Enviando ruta del archivo: examples/{input_file}")
        process.stdin.write(f"examples/{input_file}\n")
        process.stdin.flush()
        time.sleep(0.5)
        
        print(f"Debug [{thread_id}] - Enviando '2' para Solve")
        process.stdin.write("2\n")
        process.stdin.flush()
        time.sleep(0.5)
        
        print(f"Debug [{thread_id}] - Enviando '4' para Exit")
        process.stdin.write("4\n")
        process.stdin.flush()
        
        # Obtener la salida
        stdout, stderr = process.communicate(timeout=30)
        
        print(f"Debug [{thread_id}] - Comunicación completada")
        print(f"Debug [{thread_id}] - Código de salida del proceso: {process.returncode}")
        
        # Preparar el resultado
        result = {
            'file': input_file,
            'stdout': stdout,
            'stderr': stderr,
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'success': True
        }
        
        print(f"Debug [{thread_id}] - Procesado exitosamente: {input_file}")
        
    except subprocess.TimeoutExpired:
        print(f"Debug [{thread_id}] - ERROR: Timeout al procesar {input_file}")
        process.kill()
        result = {
            'file': input_file,
            'stdout': '',
            'stderr': 'ERROR: Timeout al procesar el archivo',
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'success': False
        }
    except Exception as e:
        print(f"Debug [{thread_id}] - ERROR procesando {input_file}: {str(e)}")
        print(f"Debug [{thread_id}] - Tipo de error: {type(e)}")
        result = {
            'file': input_file,
            'stdout': '',
            'stderr': f'ERROR: {str(e)}',
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'success': False
        }
    
    # Agregar el resultado a la cola
    results_queue.put(result)
    return result

def write_results_to_file(output_file):
    """
    Función que se ejecuta en un thread separado para escribir resultados al archivo
    """
    while True:
        result = results_queue.get()
        if result is None:  # Señal de terminación
            break
            
        with file_lock:
            with open(output_file, 'a') as f:
                f.write(f"\n{'='*50}\n")
                f.write(f"Resultados para el archivo: {result['file']}\n")
                f.write(f"Timestamp: {result['timestamp']}\n")
                f.write(f"{'='*50}\n")
                f.write(result['stdout'])
                if result['stderr']:
                    f.write("\nErrores:\n")
                    f.write(result['stderr'])
                f.write("\n")
        
        results_queue.task_done()

def main():
    # Ruta al programa ejecutable
    program_path = "./graph"  # Ajusta esto a la ruta de tu programa
    
    # Debug: Mostrar el directorio actual
    print(f"Debug - Directorio actual: {os.getcwd()}")
    
    # Ruta al directorio que contiene los archivos de entrada
    input_dir = "examples"
    
    # Archivo donde se guardarán todas las salidas
    output_file = "resultados_totales.txt"
    
    # Crear el directorio si no existe
    if not os.path.exists(input_dir):
        print(f"Debug - Creando directorio {input_dir}")
        os.makedirs(input_dir)
    
    # Obtener la lista de archivos .txt en el directorio
    input_files = [f for f in os.listdir(input_dir) if f.endswith('.txt')]
    
    print(f"Debug - Archivos encontrados: {input_files}")
    print(f"Se encontraron {len(input_files)} archivos para procesar")
    
    if not os.path.exists(program_path):
        print("ERROR: El programa no se encuentra en la ruta especificada")
        return

    # Iniciar el thread escritor
    writer_thread = threading.Thread(target=write_results_to_file, args=(output_file,))
    writer_thread.start()
    
    # Número de workers (puedes ajustar esto según tu CPU)
    max_workers = min(32, os.cpu_count() * 2)  # Usar máximo 32 workers o el doble de CPU cores
    print(f"Debug - Iniciando pool con {max_workers} workers")
    
    # Procesar archivos en paralelo
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        # Crear futures para cada archivo
        futures = [
            executor.submit(run_program_for_file, program_path, input_file)
            for input_file in input_files
        ]
        
        # Esperar a que todos los futures terminen
        concurrent.futures.wait(futures)
    
    # Señalar al writer thread que termine
    results_queue.put(None)
    writer_thread.join()
    
    print(f"\nProcesamiento completado. Los resultados se han guardado en {output_file}")
    
    # Mostrar estadísticas
    successful = sum(1 for future in futures if future.result()['success'])
    print(f"Archivos procesados exitosamente: {successful}/{len(input_files)}")

if __name__ == "__main__":
    main()
