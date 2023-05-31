#Interfaz para proyecto final de programación de microcontroladores
#Creado por Jorge Muñiz usando ejemplo Dashboard de PySimpleGUI
#28/05/2023

import PySimpleGUI as sg
import serial
import time
from Adafruit_IO import Client, Feed

global ser 

ser = None

ADAFRUIT_IO_KEY = 'aio_YCzt00bUNnZMwly2B6fshE7r11g2'
ADAFRUIT_IO_USERNAME = 'JorgeM726'
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

#Tema de la ventana
theme_dict = {'BACKGROUND': '#84D2F6',
                'TEXT': '#FFFFFF',
                'INPUT': '#F2EFE8',
                'TEXT_INPUT': '#000000',
                'SCROLL': '#F2EFE8',
                'BUTTON': ('#000000', '#C2D4D8'),
                'PROGRESS': ('#FFFFFF', '#C7D5E0'),
                'BORDER': 1,'SLIDER_DEPTH': 0, 'PROGRESS_DEPTH': 0}

sg.LOOK_AND_FEEL_TABLE['Dashboard'] = theme_dict
sg.theme('Dark Amber')

BORDER_COLOR = '#1B2838'
DARK_HEADER_COLOR = '#1B2838'
BPAD_TOP = ((20,20), (20, 10))
BPAD_LEFT = ((20,10), (0, 10))
BPAD_LEFT_INSIDE = (0, 10)
BPAD_RIGHT = ((10,20), (10, 20))


#Banner
top_banner = [[sg.Text('Dashboard'+ ' '*22, font='Any 40', background_color=DARK_HEADER_COLOR),
               sg.Text('Proyecto Final', font='Any 40', background_color=DARK_HEADER_COLOR)]]

#Sección superior
top  = [[sg.Text('', size=(50,1), justification='c', pad=BPAD_TOP, font='Any 20')],] 

#Bloque de control para DCs
block_2 = [[sg.Text('DC Control', font='Any 20', justification='center')],
            [sg.Slider(range=(0,127), default_value=0, size=(20,15), orientation='horizontal',font=('Helvetica', 12), key='DC1')],
            [sg.Slider(range=(0,127), default_value=0, size=(20,15), orientation='horizontal',font=('Helvetica', 12), key='DC2')]] 

#Bloque de control para servos
block_3 = [[sg.Text('Servo Control', font='Any 20')],
           [sg.Slider(range=(0,127), default_value=0, size=(20,15), orientation='horizontal',font=('Helvetica', 12), key='Servo1')],
           [sg.Slider(range=(0,127), default_value=0, size=(20,15), orientation='horizontal',font=('Helvetica', 12), key='Servo2')]]  

#Bloque de conexión para comunicación serial y Adafruit
block_4 = [[sg.Text('Comunicación Serial', font='Any 20')],
            [sg.Text('Selección de puerto'),sg.Combo(['COM1','COM2','COM3','COM4','COM5','COM6','COM7','COM8','COM9','COM10','COM11', 'COM12','COM13','COM14','COM15'], size=(10,10), default_value=('COM1'), readonly=True, key='COM_Value')],
            [sg.Button('Connect')],
            [sg.Text('Dasboard de Adafruit', font='Any 20')],
            [sg.Image(r'C:\Users\jmuni\OneDrive\Documentos\Tercer año\Primer Semestre\Progra de micros\PROYECTO2\imagen.png'),sg.Button('Adafruit')],]

            

#Integración de elementos en Layout
layout = [[sg.Column(top_banner, size=(960, 60), pad=(0,0), background_color=DARK_HEADER_COLOR)], 
          #[sg.Column(top, size=(920, 90), pad=BPAD_TOP)],
          [sg.Column([[sg.Column(block_2, size=(450,150), pad=BPAD_LEFT_INSIDE)],
                      [sg.Column(block_3, size=(450,150),  pad=BPAD_LEFT_INSIDE)]], pad=BPAD_LEFT, background_color=BORDER_COLOR),
           sg.Column(block_4, size=(450, 320), pad=BPAD_RIGHT)]]

# Create the Window
window = sg.Window('Dashboard PySimpleGUI-Style', layout, margins=(0,0),background_color=BORDER_COLOR, grab_anywhere=True, finalize=True)
window['DC1'].bind('<ButtonRelease-1>', ' Release')
window['DC2'].bind('<ButtonRelease-1>', ' Release')
window['Servo1'].bind('<ButtonRelease-1>', ' Release')
window['Servo2'].bind('<ButtonRelease-1>', ' Release')

# Event Loop to process "events" and get the "values" of the inputs
while True:
    event, values = window.read()
    if event == 'Connect': #Intentar conexión serial con valor COM del dropdown 
        try:
            ser = serial.Serial(values['COM_Value'],4800,timeout=1)
        except:
            sg.popup('No se pudo realizar la conexión')
    elif event == sg.WIN_CLOSED or event == 'Cancel': # if user closes window or clicks cancel
        break

    elif event == 'Adafruit':
        
        
        if aio.receive('selector')[3] == 'DC':
            
            ser.write("1".encode())
            time.sleep(1)
            ser.write(chr(int(aio.receive('dc1')[3])).encode(encoding="ascii"))
            time.sleep(1)

            ser.write("2".encode())
            time.sleep(1)
            ser.write(chr(int(aio.receive('dc2')[3])).encode(encoding="ascii"))
            time.sleep(1)

        elif aio.receive('selector')[3] == 'Servo':
            ser.write("3".encode())
            time.sleep(1)
            ser.write(chr(int(aio.receive('servo1')[3])).encode(encoding="ascii"))
            time.sleep(1)

            ser.write("4".encode())
            time.sleep(1)
            ser.write(chr(int(aio.receive('servo2')[3])).encode(encoding="ascii"))
            time.sleep(1)


    try: #Intentar enviar datos a través del serial dependiendo del slider que se está utilizando

        if event == 'DC1 Release':
            ser.write("1".encode())
            time.sleep(1)
            ser.write(chr(int(values['DC1'])).encode(encoding="ascii"))

        elif event == 'DC2 Release':
            ser.write("2".encode())
            time.sleep(1)
            ser.write(chr(int(values['DC2'])).encode(encoding="ascii"))

        elif event == 'Servo1 Release':
            ser.write("3".encode())
            time.sleep(1)
            ser.write(chr(int(values['Servo1'])).encode(encoding="ascii"))

        elif event == 'Servo2 Release':
            ser.write("4".encode())
            time.sleep(1)  
            ser.write(chr(int(values['Servo2'])).encode(encoding="ascii")) 
    except:
        sg.popup('Revisa la conexión serial')       
window.close()