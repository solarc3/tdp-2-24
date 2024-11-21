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
        
        stdout, stderr = process.communicate(timeout=30)
        
        print(f"Debug [{thread_id}] - Comunicación completada")
        
        result = {
            'file': input_file,
            'stdout': stdout,
            'stderr': stderr,
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'success': True
        }
        
    except Exception as e:
        print(f"Debug [{thread_id}] - ERROR procesando {input_file}: {str(e)}")
        result = {
            'file': input_file,
            'stdout': '',
            'stderr': f'ERROR: {str(e)}',
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'success': False
        }
    
    results_queue.put(result)
    return result

def write_results_to_file(output_file):
    """
    Thread dedicado a escribir resultados al archivo de salida
    """
    while True:
        result = results_queue.get()
        if result is None:
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
    # Configuración
    program_path = "./graph"  # Ajusta esto a la ruta de tu programa
    input_dir = "examples"
    output_file = "resultados_totales.txt"
    
    # Mostrar el directorio actual para debugging
    current_dir = os.getcwd()
    print(f"\nDirectorio actual: {current_dir}")
    print(f"Buscando archivos en: {os.path.join(current_dir, input_dir)}")
    
    # Verificar y crear directorio si no existe
    if not os.path.exists(input_dir):
        os.makedirs(input_dir)
        print(f"Se creó el directorio: {input_dir}")
    
    # Obtener lista de archivos .txt
    input_files = [f for f in os.listdir(input_dir) if f.endswith('.txt')]
    print(f"\nSe encontraron {len(input_files)} archivos .txt para procesar")
    
    # Verificar programa ejecutable
    if not os.path.exists(program_path):
        print(f"ERROR: El programa no se encuentra en {program_path}")
        return
    
    if not os.access(program_path, os.X_OK):
        print("AVISO: El programa no tiene permisos de ejecución")
        try:
            os.chmod(program_path, 0o755)
            print("Se agregaron permisos de ejecución al programa")
        except Exception as e:
            print(f"Error al intentar dar permisos: {e}")
    
    if len(input_files) == 0:
        print("\nNo se encontraron archivos para procesar.")
        print("Asegúrate de que:")
        print("1. El directorio 'examples' existe")
        print("2. Los archivos tienen extensión .txt")
        print("3. Tienes permisos para leer el directorio")
        return
    
    # Iniciar thread escritor
    writer_thread = threading.Thread(target=write_results_to_file, args=(output_file,))
    writer_thread.start()
    
    # Procesar archivos en paralelo
    max_workers = min(32, os.cpu_count() * 2)
    print(f"\nIniciando procesamiento con {max_workers} workers")
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = [
            executor.submit(run_program_for_file, program_path, input_file)
            for input_file in input_files
        ]
        concurrent.futures.wait(futures)
    
    # Señalar al writer thread que termine
    results_queue.put(None)
    writer_thread.join()
    
    print(f"\nProcesamiento completado. Los resultados se han guardado en {output_file}")
    
    # Mostrar estadísticas finales
    successes = sum(1 for future in futures if future.result()['success'])
    failures = len(futures) - successes
    print(f"\nEstadísticas finales:")
    print(f"Total de archivos procesados: {len(futures)}")
    print(f"Exitosos: {successes}")
    print(f"Fallidos: {failures}")

if __name__ == "__main__":
    main()
