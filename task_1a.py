
'''
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 1A of Rapid Rescuer (RR) Theme (eYRC 2019-20).
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
# Author List:  Abdul Khaliq Almel,Shashank K Holla,Naman Jain ,Amrathesh
# Filename:     task_1a.py
# Theme:        Rapid Rescuer
# Functions:    readImage, solveMaze
# 					
# Global variables:	CELL_SIZE
'''

# Import necessary modules
# Do not import any other modules
import cv2
import numpy as np
import os


# To enhance the maze image
import image_enhancer


# Maze images in task_1a_images folder have cell size of 20 pixels
CELL_SIZE = 40

#######################extra functions #############################



def findNeighbours(img,row,column,r,c):
    neighbours = []
    #############  Add your Code here   ###############
    
    cell = (img.shape[0]//r)
    row_new = int((2 * row + 1) * (cell / 2))
    col_new = int((2 * column + 1) * (cell / 2))
    top = int(row_new - (cell / 2) + 1)
    bottom = int(row_new + (cell / 2) - 2)
    left = int(col_new - (cell / 2) + 1)
    right = int(col_new + (cell / 2) - 2)
    if img[top, col_new] != 0:
        if(row-1 <r and column <c):
            neighbours.append([row - 1, column])
    if img[bottom, col_new] != 0:
        if(row+1 <r and column <c):
            neighbours.append([row + 1, column])
    if img[row_new, left] != 0:
        if(row <r and column-1 <c):
            neighbours.append([row, column - 1])
    if img[row_new, right] != 0:
        if(row <r and column+1 <c):
            neighbours.append([row, column + 1])

        
    ###################################################
    return neighbours



def buildGraph(img, no_cells_height, no_cells_width):  ## You can pass your own arguments in this space.
    graph = {}
    #############  Add your Code here   ###############
    
    cell = (img.shape[0]//no_cells_height)
    #cell=20
    
    r = int(len(img) / cell)
    c = int(len(img) / cell)
    for i in range(r):
        for j in range(c):
            aaa=findNeighbours(img, i, j,r,c)
            if(len(aaa)>0):
                graph[(i, j)] = aaa
    ###################################################

   

    return graph


def findPath(graph, initial, final, img): ## You can pass your own arguments in this space.
    #############  Add your Code here   ###############
    #print("Entered in to findPath")
    shortest = []
    visited = []
    
    for k in graph.keys():
        if (len(graph[k])==0):
            del graph[k]
    path_distance={}
    for k in graph.keys():
        path_distance[k]=[500,-1]
    
    path_distance[initial][0]=0
    current=initial
    
    while(True):
        visited.append(current)
        if(current==final):
            break
        
        for j in graph[current]:
            if(tuple(j) not in visited and path_distance[tuple(j)][0]> path_distance[current][0]+1):
                path_distance[tuple(j)][0]=path_distance[current][0]+1
                path_distance[tuple(j)][1]=current
        minn=500
        for i in path_distance.keys():
            if(path_distance[i][0]<minn and i not in visited):
                minn=path_distance[i][0]
                current=i
    
    nextt=final
    while(nextt!=initial):
        shortest.append(nextt)
        nextt=path_distance[nextt][1]
    shortest.append(nextt)
    shortest.reverse()
    return(shortest)



    ###################################################




##################### end of extras #################################

'''
* Function Name: readImage
* Input: img_file_path
* Output: binary_img 
* Logic: openCV used
* Example Call: binary_img = readImage(img_file_path)
'''

def readImage(img_file_path):
    img = cv2.imread(img_file_path)
    gimg= cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    ret, binary_img = cv2.threshold(gimg, 32, 255, cv2.THRESH_BINARY)
    return binary_img

'''
* Function Name: solveMaze
* Input: `original_binary_img` :	[ numpy array ]
		binary form of the original image at img_file_path
	`initial_point` :		[ tuple ]
		start point coordinates
	`final_point` :			[ tuple ]
		end point coordinates
	`no_cells_height` :		[ int ]
		number of cells in height of maze image
	`no_cells_width` :		[ int ]
		number of cells in width of maze image
* Output:`shortestPath` :		[ list ]
		list of coordinates of shortest path from initial_point to final_point
* Logic: Dijkstra's Algorithm used
* Example Call:shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)
'''


def solveMaze(img, initial_point, final_point, no_cells_height, no_cells_width):

	"""
	Purpose:
	---
	the function takes binary form of original image, start and end point coordinates and solves the maze
	to return the list of coordinates of shortest path from initial_point to final_point
	"""
	
	#shortestPath = []

	#############	Add your Code here	###############

	#print("entering to slove maze")
	graph = buildGraph(img, no_cells_height, no_cells_width)
	shortestPath = findPath(graph, initial_point, final_point, img) 
	#print("find path completed")
	#print(shortestPath)
	#print(type(shortestPath))

	###################################################
	
	return shortestPath


#############	You can add other helper functions here		#############



#########################################################################
'''

# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
# 
# Function Name:	main
# Inputs:			None
# Outputs: 			None
# Purpose: 			the function first takes 'maze00.jpg' as input and solves the maze by calling readImage
# 					and solveMaze functions, it then asks the user whether to repeat the same on all maze images
# 					present in 'task_1a_images' folder or not
'''
if __name__ == '__main__':

	curr_dir_path = os.getcwd()
	img_dir_path = curr_dir_path + '/../task_1a_images/'				# path to directory of 'task_1a_images'
	
	file_num = 0
	img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'		# path to 'maze00.jpg' image file

	print('\n============================================')

	print('\nFor maze0' + str(file_num) + '.jpg')

	#i=readImage(img_file_path)
	#print(i)

	#print(buildGraph(i))

	try:
		
		original_binary_img = readImage(img_file_path)
		aaa= original_binary_img.shape
		height =aaa[0]
		width =aaa[1]

	except AttributeError as attr_error:
		
		print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
		exit()
	
	no_cells_height = int(height/CELL_SIZE)							# number of cells in height of maze image
	no_cells_width = int(width/CELL_SIZE)							# number of cells in width of maze image
	initial_point = (0, 0)											# start point coordinates of maze
	final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze

	try:

		shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)

		if len(shortestPath) > 2:

			img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)
			#img = original_binary_img
			
		else:

			print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
			exit()
	
	except TypeError as type_err:
		
		print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
		exit()

	print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))
	
	print('\n============================================')
	
	cv2.imshow('canvas0' + str(file_num), img)
	cv2.waitKey(0)
	cv2.destroyAllWindows()

	choice = input('\nWant to run your script on all maze images ? ==>> "y" or "n": ')

	if choice == 'y':

		file_count = len(os.listdir(img_dir_path))

		for file_num in range(file_count):

			img_file_path = img_dir_path + 'maze0' + str(file_num) + '.jpg'

			print('\n============================================')

			print('\nFor maze0' + str(file_num) + '.jpg')

			try:
				
				original_binary_img = readImage(img_file_path)
				height, width = original_binary_img.shape

			except AttributeError as attr_error:
				
				print('\n[ERROR] readImage function is not returning binary form of original image in expected format !\n')
				exit()
			
			no_cells_height = int(height/CELL_SIZE)							# number of cells in height of maze image
			no_cells_width = int(width/CELL_SIZE)							# number of cells in width of maze image
			initial_point = (0, 0)											# start point coordinates of maze
			final_point = ((no_cells_height-1),(no_cells_width-1))			# end point coordinates of maze

			try:

				shortestPath = solveMaze(original_binary_img, initial_point, final_point, no_cells_height, no_cells_width)

				if len(shortestPath) > 2:

					img = image_enhancer.highlightPath(original_binary_img, initial_point, final_point, shortestPath)
					
				else:

					print('\n[ERROR] shortestPath returned by solveMaze function is not complete !\n')
					exit()
			
			except TypeError as type_err:
				
				print('\n[ERROR] solveMaze function is not returning shortest path in maze image in expected format !\n')
				exit()

			print('\nShortest Path = %s \n\nLength of Path = %d' % (shortestPath, len(shortestPath)))
			
			print('\n============================================')

			cv2.imshow('canvas0' + str(file_num), img)
			cv2.waitKey(0)
			cv2.destroyAllWindows()
	
	else:

		print('')


