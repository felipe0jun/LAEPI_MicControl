import requests
import time

BASE_URL = 'http://192.168.1.2/'

def move_motor(motor_number : int, degrees : int) -> None:
    """
    Move um motor específico para a posição indicada em graus.
    
    Args:
    - motor_number (int): Número do motor (1 ou 2).
    - degrees (int): Quantidade de graus para mover. Deve ser múltiplo
    de 5.
    
    Raises:
    - ValueError: Se o motor_number não for 1 ou 2.
    - ValueError: Se degrees não for múltiplo de 5.
    
    Returns:
    - None.
    """
    if motor_number not in [1,2]:
        raise ValueError('Os motores devem ser número 1 ou número 2.')
    elif not degrees%5 == 0:
        raise ValueError('Os graus devem ser múltiplos de 5.')
    else:
        response = requests.get(f'{BASE_URL}?motor_{motor_number}={degrees}')

def reset_motor(motor_number : int) -> None:
    """
    Reseta um motor específico para sua posição inicial.
    
    Args:
    - motor_number (int): Número do  motor (1 ou 2).
    
    Raises:
    - ValueError: Se o motor_number não for 1 ou 2.
    
    Returns:
    - None.
    """
    if motor_number not in [1,2]:
        raise ValueError('Os motores devem ser número 1 ou número 2.')
    else:
        response = requests.get(f'{BASE_URL}motor_{motor_number}_reset?')
        
def fake_sound_emission(seconds : int) -> None:
    """
    Simula a emissão de um som, aguardando por um tempo definido.
    
    Args:
    - seconds (int): Duração da emissão em segundos.
    
    Returns:
    - None.
    """
    time.sleep(seconds)