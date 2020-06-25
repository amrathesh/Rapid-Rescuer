
'''
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 1C of Rapid Rescuer (RR) Theme (eYRC 2019-20).
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
# Filename:     task_1c.py
# Functions:    computeSum
# Global variables:	None
					
'''
# Import necessary modules
import cv2
import numpy as np
import os
import sys



#############	You can import other modules here	#############



from keras.datasets import mnist
from keras.utils import to_categorical
from keras.models import Sequential
from keras.layers import Conv2D
from keras.layers import MaxPooling2D
from keras.layers import Dense
from keras.layers import Flatten
from keras.optimizers import SGD

from keras.preprocessing.image import load_img
from keras.preprocessing.image import img_to_array
from keras.models import load_model

from google_drive_downloader import GoogleDriveDownloader as gdd


#import train_digit_classifier as train


#################################################################

# load and prepare the image
def load_image(filename):
	# load the image
	img = load_img(filename, grayscale=True, target_size=(28, 28))
	# convert to array
	img = img_to_array(img)
	#print(img.shape)
	# reshape into a single sample with 1 channel
	img = img.reshape(1, 28, 28, 1)
	# prepare pixel data
	img = img.astype('float32')
	img = img / 255.0
	return img

# load an image and predict the class
def run_example():
	# load the image
	img = load_image('sample_image.jpg')
	# load model
	model = load_model('final_model.h5')
	# predict the class
	digit = model.predict_classes(img)
	print(digit[0])



gdd.download_file_from_google_drive(file_id='1gc1EfmGyBMoYwbMuEgxg33EShkSsnqa9',dest_path=os.getcwd()+'/model.h5')

model = load_model('model.h5')
##classifier=usingSk()
val={}

'''
# Function Name:	computeSum
# Purpose:              the function takes file path of original image and shortest path as argument and returns list of digits, digits on path and sum of digits on path

# Inputs: 		img_file_path [ file path of image ]

# 			shortestPath [ list of coordinates of shortest path from initial_point to final_point ]
# Outputs:		digits_list [ list of digits present in the maze image ]
# 			digits_on_path [ list of digits present on the shortest path in the maze image ]
# 			sum_of_digits_on_path [ sum of digits present on the shortest path in the maze image ]

# Purpose: 		the function takes file path of original image and shortest path in the maze image
# 			to return the list of digits present in the image, list of digits present on the shortest
# 			path in the image and sum of digits present on the shortest	path in the image

# Logic:	        Using Brute force approach to find combination of digits whose sum is equal to total vacancies.

# Example call: 	digits_list, digits_on_path, sum_of_digits_on_path = computeSum(img_file_path, shortestPath)

'''

def computeSum(img_file_path, shortestPath):

	
	digits_list = []
	digits_on_path = []
	sum_of_digits_on_path = 0


	#############  Add your Code here   ###############
	
	img = task_1a.readImage(img_file_path)
	cellSize = (task_1a.CELL_SIZE)
	r = img.shape[0] // cellSize
	c = img.shape[1] // cellSize

	global val
	for i in range(r):
		for j in range(c):
			
			
			imgx = img[i*cellSize+4:(i+1)*cellSize-4,(j)*cellSize+4: (j+1)*cellSize-4]
			
			imgx = cv2.bitwise_not(imgx)

			count=0
			for ii in imgx:
				for jj in ii:
					if(jj !=0):
						count=count+1
			#print(imgx)
			if(count>8):
				cv2.imwrite('temp.jpg',imgx)
				imgp=load_image('temp.jpg')

				imgx = cv2.normalize(imgp, None, 0 , 16, cv2.NORM_MINMAX, dtype=cv2.CV_64F)
				#imgx = cv2.resize(imgx, (28,28))

				np.set_printoptions(threshold=sys.maxsize)
				digit = model.predict_classes(imgx)
				
				#print("\n Prediction:",digit[0])
				val[tuple((i,j))]= digit[0]
				digits_list.append(int(digit[0]))
				#cv2.imshow("",imgx)
				#res = cv2.waitKey(0)
				
	#print(val)
	#print(shortestPath)
	height, width = img.shape
	no_cells_height = int(height/task_1a.CELL_SIZE)					# number of cells in height of maze image
	no_cells_width = int(width/task_1a.CELL_SIZE)
	for kk in val.keys():
		r=kk[0]
		c=kk[1]
		ne=task_1a.findNeighbours(img,r,c,no_cells_height,no_cells_width)
		#print(ne)
		if(kk in shortestPath):
			digits_on_path.append(int(val[kk]))
			continue
		for nn in ne:
			if tuple((nn[0],nn[1])) in shortestPath:
				digits_on_path.append(int(val[kk]))
				break		
	sum_of_digits_on_path=sum(digits_on_path)		
			
	
	###################################################

	return digits_list, digits_on_path, sum_of_digits_on_path


#############	You can add other helper functions here		#############
def returnVal():
	global val
	return val


#########################################################################

'''
# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
# 
# Function Name:	main
# Inputs:			None
# Outputs: 			None
# Purpose: 			the function first takes 'maze00.jpg' as input and solves the maze by calling computeSum
# 					function, it then asks the user whether to repeat the same on all maze images
# 					present in 'task_1c_images' folder or not
'''
if __name__ != '__main__':
	
	curr_dir_path = os.getcwd()

	# Importing task_1a and image_enhancer script
	try:

		task_1a_dir_path = curr_dir_path + '/../../Task 1A/codes'
		sys.path.append(task_1a_dir_path)

		import task_1a
		import image_enhancer

	except Exception as e:

		print('\ntask_1a.py or image_enhancer.pyc file is missing from Task 1A folder !\n')
		exit()

if __name__ == '__main__':
	
	curr_dir_path = os.getcwd()
	img_dir_path = curr_dir_path + '/../task_1c_images/'				# path to directory of 'task_1c_images'
	
	file_num = 0
	img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'		# path to 'maze00.jpg' image file

	# Importing task_1a and image_enhancer script
	try:

		task_1a_dir_path = curr_dir_path + '/../../Task 1A/codes'
		sys.path.append(task_1a_dir_path)

		import task_1a
		import image_enhancer

	except Exception as e:

		print('\n[ERROR] task_1a.py or image_enhancer.pyc file is missing from Task 1A folder !\n')
		exit()
	
	# modify the task_1a.CELL_SIZE to 40 since maze images
	# in task_1c_images folder have cell size of 40 pixels
	task_1a.CELL_SIZE = 40

	print('\n============================================')

	print('\nFor maze0' + str(file_num) + '.jpg')

	try:
		
		original_binary_img = task_1a.readImage(img_file_path)
		height, width = original_binary_img.shape

	except AttributeError as attr_error:
		
		print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
		exit()

	
	no_cells_height = int(height/task_1a.CELL_SIZE)					# number of cells in height of maze image
	no_cells_width = int(width/task_1a.CELL_SIZE)					# number of cells in width of maze image
	initial_point = (0, 0)											# start point coordinates of maze
	final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze
	
	try:

		shortestPath = task_1a.solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)
		
		if len(shortestPath) > 2:
			
			img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)
			
		else:

			print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
			exit()
	
	except TypeError as type_err:
		
		print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
		exit()

	print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))

	digits_list, digits_on_path, sum_of_digits_on_path = computeSum(img_file_path, shortestPath)

	print('\nDigits in the image = ', digits_list)
	print('\nDigits on shortest path in the image = ', digits_on_path)
	print('\nSum of digits on shortest path in the image = ', sum_of_digits_on_path)

	print('\n============================================')

	cv2.imshow('canvas0' + str(file_num), img)
	cv2.waitKey(0)
	cv2.destroyAllWindows()

	choice = input('\nWant to run your script on all maze images ? ==>> "y" or "n": ')

	if choice == 'y':

		file_count = len(os.listdir(img_dir_path))

		for file_num in range(file_count):

			img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'		# path to 'maze00.jpg' image file

			print('\n============================================')

			print('\nFor maze0' + str(file_num) + '.jpg')

			try:
				
				original_binary_img = task_1a.readImage(img_file_path)
				height, width = original_binary_img.shape

			except AttributeError as attr_error:
				
				print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
				exit()

			
			no_cells_height = int(height/task_1a.CELL_SIZE)					# number of cells in height of maze image
			no_cells_width = int(width/task_1a.CELL_SIZE)					# number of cells in width of maze image
			initial_point = (0, 0)											# start point coordinates of maze
			final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze

			try:

				shortestPath = task_1a.solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)
				
				if len(shortestPath) > 2:
						
					img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)
					
				else:

					print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
					exit()
			
			except TypeError as type_err:
				
				print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
				exit()

			print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))

			digits_list, digits_on_path, sum_of_digits_on_path = computeSum(img_file_path, shortestPath)

			print('\nDigits in the image = ', digits_list)
			print('\nDigits on shortest path in the image = ', digits_on_path)
			print('\nSum of digits on shortest path in the image = ', sum_of_digits_on_path)

			print('\n============================================')

			cv2.imshow('canvas0' + str(file_num), img)
			cv2.waitKey(0)
			cv2.destroyAllWindows()

	else:

		print('')


