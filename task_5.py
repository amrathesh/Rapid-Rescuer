
'''
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 5 of Rapid Rescuer (RR) Theme (eYRC 2019-20).
*
*  This software is made available on an "AS IS WHERE IS BASIS".
*  Licensee/end user indemnifies and will keep e-Yantra indemnified from
*  any and all claim(s) that emanate from the use of the Software or
*  breach of the terms of this agreement.
*
*  e-Yantra - An MHRD project under National Mission on Education using ICT (NMEICT)
*
*****************************************************************************************
'''
'''

# Team ID:      34
# Author List:  Abdul Khaliq Almel,Shashank K Holla,Naman Jain,Amrathesh
# Filename:	task_5.py
# Functions:	connect_to_server, send_to_receive_from_server, find_new_path, create_combination_dict,
# 		python_client, take_input_for_reposition_restart, service_shutdown
# 					
# Global variables:	SERVER_IP, SERVER_PORT, SERVER_ADDRESS, sock

'''
# Import necessary modules
# Do not import any other modules
import cv2
import socket
import sys
import os
from collections import defaultdict, Counter
import signal, threading


# IP address of robot-server (ESP32)
SERVER_IP = '192.168.4.1'

# Port number assigned to server
SERVER_PORT = 3333
SERVER_ADDRESS = (SERVER_IP, SERVER_PORT)

# global 'sock' variable for socket connection
sock = None

start_end = []
c_f = 0

        '''

        Function Name: connect_to_server
        
	Purpose: the function creates socket connection with server

	Input  : SERVER_ADDRESS : [ tuple ][port address of server]

	Output : sock :	[ object of socket class ] [object of socket class for socket communication]

	Example Call: sock = connect_to_server(SERVER_ADDRESS)

	'''

def connect_to_server(SERVER_ADDRESS):


	global sock
	sock = None

	#############  Add your Code here   ###############

	sock=socket.socket()
	print("Connecting.....")
	sock.connect(SERVER_ADDRESS)
	print("CONNECTED")

	###################################################

	return sock


	'''
        Function Name: send_to_receive_from_server
        
	Purpose: the function sends / receives data to / from server in proper format

	Input Arguments: sock [ object of socket class ] [object of socket class for socket communication]
	                 string [ string ] [data to be sent from client to server]

	Outputs : sent_data [ string ] [data sent from client to server in proper format]
	         recv_data [ string ] [data sent from server to client in proper format]

	Example call: sent_data, recv_data = send_to_receive_from_server(sock, shortestPath)

	'''

def send_to_receive_from_server(sock, data_to_send):

	sent_data = ''
	recv_data = ''

	#############  Add your Code here   ###############

	sent_data = ' #' + str(data_to_send) + '#'+'\0'
	sock.send(sent_data.encode())
	recv_data = sock.recv(1024)
	recv_data = recv_data.decode()

	###################################################

	return sent_data, recv_data

'''
        Function name: find_new_path
        
	Purpose: the function computes new shortest path from cell adjacent to obstacle to final_point

	Input Arguments:
	recv_data :	[ string ]
		data sent from server to client in proper format
	shortestPath`	:	[ list ]
		list of coordinates of shortest path from initial_point to final_point

	Outputs:
	---
	`obstacle_coord` :	[ tuple ]
		position of dynamic obstacle in (x,y) coordinate
	`new_shortestPath` :	[ list ]
		list of coordinates of shortest path from new_initial_point to final_point
	`new_initial_point` :	[ tuple ]
		coordinate of cell adjacent to obstacle for the new shortest path
	`img` :	[ numpy array ]

	Example call: obstacle_coord, new_shortestPath, new_initial_point, img = find_new_path(recv_data, shortestPath)

	'''

def find_new_path(recv_data, shortestPath):

	obstacle_coord = ()
	new_shortestPath = []
	new_initial_point = ()

	global img_file_path, final_point, no_cells_height, no_cells_width

	#############  Add your Code here   ###############

	open_brack_idx = recv_data.find('(')
	comma_idx = recv_data.find(',')
	close_brack_idx = recv_data.find(')')

	if abs((open_brack_idx - comma_idx)) > 2:
		obstacle_x = (int(recv_data[comma_idx-2]))*10 + (int(recv_data[comma_idx-1]))
	else:
		obstacle_x = (int(recv_data[comma_idx-1]))
	
	if abs((close_brack_idx - comma_idx)) > 2:
		obstacle_y = (int(recv_data[comma_idx+1]))*10 + (int(recv_data[comma_idx+2]))
	else:
		obstacle_y = (int(recv_data[comma_idx+1]))
	
	obstacle_coord =tuple(map(int,recv_data.strip('@').strip('(').strip(')').split(',')))

	#obstacle_coord=tuple(obstacle_x,obstacle_y)
	obstacle_list.append(obstacle_x)
	obstacle_list.append(obstacle_y)


	# colour the cell as blocked at the obstacle's position
	if obstacle_coord in shortestPath:

		obstacle_index = shortestPath.index((obstacle_x, obstacle_y))
		new_initial_point = shortestPath[obstacle_index-1] # (0, 0)                  # start coordinates of maze
	
		img = task_1a.readImage(img_file_path)

		obstacle_pos = 0

		while obstacle_pos < len(obstacle_list):
		# print(obstacle_pos, len(obstacle_list), obstacle_list[obstacle_pos], obstacle_list[obstacle_pos+1])
			image_enhancer.colourCell(img, obstacle_list[obstacle_pos], obstacle_list[obstacle_pos+1], 0)
			obstacle_pos = obstacle_pos + 2
		# cv2.imshow('colored' + str(obstacle_pos), img)
	
		new_shortestPath = task_1a.solveMaze(img, new_initial_point, final_point, no_cells_height, no_cells_width)
		#print(new_shortestPath)
	else:
		new_shortestPath=shortestPath
		new_initial_point=shortestPath[0]
		img = task_1a.readImage(img_file_path)


	###################################################

	return obstacle_coord, new_shortestPath, new_initial_point, img


#############	You can add other helper functions here		#############



#########################################################################


# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
def create_combination_dict(combination_digits, combination_locations):

	'''
        Function name: create_combination_dict
        
	Purpose:
	---
	the function takes digits and locations and returns appropriate dictionary for combination chosen,
	it handles the existence of two duplicate keys (digits) in a dictionary

	Input Arguments:
	---
	`combination_digits` :	[ list of integers ]
		list of digits chosen in the combination of Sum
	`combination_locations`	:	[ list of tuples ]
		list of locations in maze image of digits chosen in the combination of Sum

	Returns:
	---
	`combination` :	[ dict ]
		appropriate dictionary for the combination of digits chosen for the Sum,
		it handles the existence of two duplicate keys (digits) in a dictionary

	Example call:
	---
	combination = create_combination_dict(combination_digits, combination_locations)

	'''

	combination = defaultdict(list)
	loc = 0

	combination_digits_count = Counter(combination_digits)

	for digit in combination_digits:
		if combination_digits_count[digit] > 1:
			combination[digit].append(combination_locations[loc])
		else:
			combination[digit] = combination_locations[loc]
		loc = loc + 1
	
	combination.default_factory = None
	combination = dict(combination)

	return combination


# NOTE:	YOU ARE ALLOWED TO MAKE CHANGE TO THIS FUNCTION ONLY WHERE SPECIFIED BELOW

        '''

        Function Name: python_client
        
	Purpose: To communicate between client and server

	Input  : server address

	Output : sock

	Example Call: sock = connect_to_server(SERVER_ADDRESS)

	'''
def python_client():

	try:

		curr_dir_path = os.getcwd()
		img_dir_path = curr_dir_path + '/../Maze_Image/'					# path to directory of 'Maze_Image'

		# for Original Configuration
		img_file_path = img_dir_path + '/original_config_maze.jpg'		# path to 'original_config_maze.jpg' image file

		# for Bonus Configuration
		# img_file_path = img_dir_path + '/bonus_config_maze.jpg'			# path to 'bonus_config_maze.jpg' image file

		# Sum for Original configuration
		sum_integer = 8

		# Sum for Bonus configuration
		# sum_integer = 10

		# Importing task_1a and image_enhancer script
		try:

			task_1a_dir_path = curr_dir_path
			sys.path.append(task_1a_dir_path)

			import task_1a
			import image_enhancer

			# changing the 'CELL_SIZE' variable to 40 x 40 pixels in accordance with the size in image
			task_1a.CELL_SIZE = 40

		except Exception as e:
			print('\n[ERROR] task_1a.py or image_enhancer.pyc files are missing from this folder !\n')
			exit()
		
		# Importing task_1c script
		try:

			import task_1c
			
		except ImportError as imp_error:

			print('\n[ERROR] task_1c.py file is not present in this folder !\n')
		
		except Exception as e:
			
			print('\n[ERROR] task_1c.py file contains some error, check your code !\n')
			raise(e)
			exit()

		# Importing task_2 script
		try:

			import task_2
			
		except ImportError as imp_error:

			print('\n[ERROR] task_2.py file is not present in this folder !\n')
		
		except Exception as e:
			
			print('\n[ERROR] task_2.py file contains some error, check your code !\n')
			raise(e)
			exit()
		
		# Read the image using readImage function from task_1a
		try:
			original_binary_img = task_1a.readImage(img_file_path)
			height, width = original_binary_img.shape

		except AttributeError as attr_err:
			print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
			exit()

		no_cells_height = int(height/task_1a.CELL_SIZE)					# number of cells in height of maze image
		no_cells_width = int(width/task_1a.CELL_SIZE)					# number of cells in width of maze image
		
		# Creating socket connection with the server on robot
		try:

			print('\n============================================')

			# Create socket connection with server
			try:
				sock = connect_to_server(SERVER_ADDRESS)
			
				if sock == None:
					print('\n[ERROR] connect_to_server function is not returning socket object in expected format !\n')
					exit()

				else:
					print('\nConnecting to %s Port %s' %(SERVER_ADDRESS))

			except ConnectionRefusedError as connect_err:
				print('\n[ERROR] the robot might be OFF, start the server first !\n')
				exit()
			
			# Get the digits_list using computeSum function from task_1c
			digits_list = task_1c.computeSum(img_file_path,[])[0]
			val = task_1c.returnVal()
			
			digits_list=[]
			positions_list=[]
			digitLoc = []
			for c in val.keys():
				digitLoc.append([val[c],c[0],c[1]])
				digits_list.append(val[c])
				positions_list.append(tuple([c[0],c[1]]))
				


			print(digitLoc)
			# Get the combination_of_digits using findCombination function from task_2
			firezones = task_2.findCombination(digitLoc, sum_integer)

			firezones = sorted(firezones, key = lambda x : x[3])
			#############  NOTE: Edit this part to complete the Task 5 implementation   ###############

			
			# Get the combination_locations from the maze image for the digits chosen in combination_of_digits
			combination_locations = []
			combination_of_digits = []
			for c in firezones:
				combination_of_digits.append(c[0])
				combination_locations.append(tuple((c[1],c[2])))	

			combination = create_combination_dict(combination_of_digits, combination_locations)
			print(combination)
			print('\nGiven Digits in image = %s \n\nGiven Combination of Digits with Locations = %s' % (digits_list, combination))

			start_end.append((0,0))
			victims=[]
			for i in combination.keys():
				victims.append(i)
				start_end.append(combination[i])

			start_end.append((9,9))
			victims.append(0)
			print(start_end)
			print("Waiting for start")
			print(sock)
			rcv=sock.recv(1024).decode()
			print(rcv)
			global c_f
			start=(0,0)
			if("started" in rcv) :
				digitsSend = "#" + str(digits_list) + "#"
				sock.send(digitsSend.encode())
				rcv=sock.recv(1024).decode()
				print(rcv)

				combinationSend = "#" + str(combination) + "#"
				sock.send(combinationSend.encode())
				ok=sock.recv(1024).decode()
				print(ok)
				while(c_f<len(start_end)-1):
					original_binary_img = task_1a.readImage(img_file_path)

					print(start, start_end[c_f+1])
					x = task_1a.solveMaze(original_binary_img,start, start_end[c_f+1], no_cells_height, no_cells_width)
					cv2.imshow('',original_binary_img)
					cv2.waitKey(0)
					start=x[-2]
					s = ""
					for a in x:
						s+=str(a[0]) + str(a[1])
					s=s+'-'+str(victims[c_f])
					print(s)
					sent,recv=send_to_receive_from_server(sock, str(x))

					print("sent",sent)
					print("recv",recv)
					if('obstacle' in recv ):
						start=tuple(list(map(int,recv[recv.find('(')+1:recv.find(')')].split(','))))
						continue
					else:
						c_f+=1
						

				while(True):
					sent,recv=send_to_receive_from_server(sock, s)
					if('accomplished' in recv):
						break
					
				
			



			##########################################################################################
			
		except Exception as e:
			
			print('\n[ERROR] Some error occurred, check your code !\n')
			raise(e)
			exit()
	
	except KeyboardInterrupt:

		sys.exit()


# NOTE:	YOU ARE ALLOWED TO EDIT THIS FUNCTION
def take_input_for_reposition_restart():

	global sock

	try:

		while True:

			data_to_send = input('\nEnter either "%" for Restart OR "&" for Reposition: \n')

			if (data_to_send == '%') or (data_to_send == '&'):
				
				if (data_to_send == '%'):

					print('\nOne Restart for the run is taken !')

					sent_data = str(data_to_send)
					sock.sendall(sent_data.encode())

					print('\nClosing Socket')

					sock.close()

					sys.exit()

				elif (data_to_send == '&'):

					print('\nOne Reposition for the run is taken !')
				
					sent_data = str(data_to_send)
					sock.sendall(sent_data.encode())

					#############  NOTE: Edit this part to complete the Task 4 implementation   ###############
					#sock.send("&\0".encode())
                                        recv=sock.recv(1024).decode()
                                        if("reposition" in recv):
                                                x,y=map(int,recv[recv.find('(')+1:recv.find(')')].split(','))
                                                print(x,y)


					

					##########################################################################################
			
			else:

				print('\nYou must enter either "%" OR "&" only !')

	except KeyboardInterrupt:
		
		print('\nClosing Socket')
		sock.close()

		sys.exit()


# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS CLASS
class ServiceExit(Exception):
	"""
	Custom exception which is used to trigger the clean exit
	of all running threads and the main program.
	"""
	pass
 

# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
def service_shutdown(signum, frame):
	print('Caught signal %d' % signum)
	raise ServiceExit

'''
# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
#
# Function Name:	main
# Inputs:			None
# Outputs: 			None
# Purpose: 			the function creates two independent Threads, one for running the Python Client program,
# 					second to take the user input for Restart ( " % " ) or Reposition ( " & " ) as per the Rulebook
'''

if __name__ == '__main__':

	try:

		# Register the signal handlers
		signal.signal(signal.SIGTERM, service_shutdown)
		signal.signal(signal.SIGINT, service_shutdown)
		
		# creating thread
		t1 = threading.Thread(target=python_client, name='python_client')
		t2 = threading.Thread(target=take_input_for_reposition_restart, name='take_input_for_reposition_restart')

		t1.shutdown_flag = threading.Event()
		t2.shutdown_flag = threading.Event()

		# starting thread 1
		t1.start()
		# starting thread 2
		t2.start()
		
	except ServiceExit:

		t1.shutdown_flag.set()
		t2.shutdown_flag.set()

		# wait until thread 1 is completely executed
		t1.join(1)
		# wait until thread 2 is completely executed
		t2.join(1)

		# both threads completely executed
		print("Done!")

