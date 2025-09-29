from api_package.aux_functions import move_motor,reset_motor,fake_sound_emission

def exec_pipeline(seconds_to_measure : int = 3) -> None:
    """
    Executa o pipeline completo de medições com movimentação 
    dos motores.
    
    A função realiza uma sequência de etapas, em que:
    - Os motores M1 e M2 são posicionados em diferentes ângulos.
    - Em cada posição é feita uma simulação de medição utilizando
    `fake_sound_emission`.
    
    O pipeline segue uma ordem fixa de movimentos, alterando entre
    motor 1 e motor 2.
    
    Args:
    - seconds_to_measure (int, opcional): Tempo de medição em cada
    etapa, em segundos. Padrão é setado como 3.
    
    Returns:
    - None.
    """
    print('Executando medição em M1 = 0 graus e M2 = 0 graus.')
    fake_sound_emission(seconds_to_measure)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Solicitando movimento do motor 1 em +90 graus')
    move_motor(1,95) # 5 graus a mais porque estou começando do ponto de referência que está levemente a -5 graus do eixo central.
    print('Fim da execução','-'*100,sep='\n')
    
    print('Executando medição em M1 = 90 graus e M2 = 0 graus.')
    fake_sound_emission(seconds_to_measure)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Solicitando movimento do motor 2 em +90 graus')
    move_motor(2,90)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Executando medição em M1 = 90 graus e M2 = 90 graus.')
    fake_sound_emission(seconds_to_measure)
    print('Fim da execução','-'*100,sep='\n')

    print('Solicitando movimento do motor 2 em +90 graus')
    move_motor(2,90)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Executando medição em M1 = 90 graus e M2 = 180 graus.')
    fake_sound_emission(seconds_to_measure)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Solicitando movimento do motor 2 em +90 graus')
    move_motor(2,90)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Executando medição em M1 = 90 graus e M2 = 270 graus.')
    fake_sound_emission(seconds_to_measure)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Solicitando movimento do motor 1 em +90 graus')
    move_motor(1,90)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Executando medição em M1 = 180 graus e M2 = 0 graus.')
    fake_sound_emission(seconds_to_measure)
    print('Fim da execução','-'*100,sep='\n')
    
    print('Solicitando reset do motor 2')
    reset_motor(2)
    print('Fim da execução','-'*100,sep='\n')

    print('Solicitando reset do motor 1')
    reset_motor(1)
    print('Fim da execução','-'*100,sep='\n')