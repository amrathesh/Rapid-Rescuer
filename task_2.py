
'''
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 2 of Rapid Rescuer (RR) Theme (eYRC 2019-20).
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
# Author List:  Abdul Khaliq Almel,Shashank K Holla
# Filename:     task_2.py
# Functions:	findCombination
# Global variables: None
'''

# Import necessary modules
# Do not import any other modules
import cv2
import numpy as np
import os

combination_of_digits = []

def f(digitsList, sumInteger, i, x, n):
    #print(x, i)
    if(i == n):
        s = 0
        for c in x:
            s += c[0]
            
        if(s == sumInteger):
            combination_of_digits.append(x)
        return 0
    else:
        s = 0
        for c in x:
            s += c[0]
        
        if(s == sumInteger):
            combination_of_digits.append(x)
        elif (s > sumInteger):
            return 0
        else:
            f(digitsList, sumInteger, i+1, list(x), n)
            x.append(digitsList[i])
            # print("appending", x) 
            f(digitsList, sumInteger, i+1, list(x), n)
            

 '''
    # Function Name: findCombination
    
    Purpose:  the function takes list of Digits and integer of Sum and returns the list of combination of digits

    Inputs :  Digits_list [list of Digits on the first line of text file]
                        Sum_integer [integer of Sum on the second line of text file]

    Outputs: combination_of_digits [list of digits whose total is equal to Sum_integer]

    Logic: Brute force
     
    Example call: combination_of_digits = findCombination(Digits_list, Sum_integer)

    '''

def findCombination(Digits_list, Sum_integer):

    
    global combination_of_digits
    combination_of_digits = []
    #############	Add your Code here	###############
    n = len(Digits_list)
    i = 0
    digitsListSq = []

    ## The fourth parameter is the eucledian distance from 0,0. 
    ## This is used to calculate the distance from origin
    for c in Digits_list:
        digitsListSq.append([c[0],c[1], c[2], c[1]**2 + c[2]**2])

    x = []
    digitsListSq = sorted(digitsListSq, reverse=True, key=lambda x: (x[0], -x[2]))
    f(digitsListSq, Sum_integer, i, x, n)
    #print("Comb: ", combination_of_digits)
    l = len(combination_of_digits[0])
    x = combination_of_digits[0]
    for i in combination_of_digits:
        #print(i, len(i),l)
        if (len(i) <= l) :
            #print(i, l, "Changing")
            l = len(i)
            x = i
    
    combination_of_digits = x
    ###################################################

    return combination_of_digits


#############	You can add other helper functions here		#############



#########################################################################

'''
# NOTE:	YOU ARE NOT ALLOWED TO MAKE ANY CHANGE TO THIS FUNCTION
# 
# Function Name:	main
# Inputs:			None
# Outputs: 			None
# Purpose: 			the function first takes 'Test_input0.txt' as input, finds the combination
# 					of digits and selects one of them using findCombination function, it then asks
#                   the user whether to repeat the same on other Test input txt files
#                   present in 'Test_inputs' folder or not

'''
if __name__ == '__main__':

    curr_dir_path = os.getcwd()
    txt_input_dir_path = curr_dir_path + '/Test_inputs/'				            # path to directory of 'Test_inputs'

    file_num = 0
    txt_file_path = txt_input_dir_path + 'Test_input' + str(file_num) + '.txt'      # path to 'Test_input0.txt' text file

    txt_input_obj = open(txt_file_path, 'r')

    print('\n============================================')

    print('\nFor Test_input' + str(file_num) + '.txt')

    Digits_list = [int(digit) for digit in txt_input_obj.readline().split()]
    Sum_integer = [int(integer) for integer in txt_input_obj.readline().split()][0]

    print('\nGiven List of Digits = %s \n\nGiven Integer of Sum = %d' % (Digits_list, Sum_integer))

    try:
        
        combination_of_digits = findCombination(Digits_list, Sum_integer)

    except Exception as e:
        
        print('\n[ERROR] findCombination function ran into an error, check your code !\n')
        raise(e)
        exit()
    
    print('\nCombination of digits chosen = %s \n\nNumber of digits chosen = %d' % (combination_of_digits, len(combination_of_digits)))
    
    print('\n============================================')
    
    choice = input('\nWant to run your script on other text input files ? ==>> "y" or "n": ')

    if choice == 'y':

        file_count = len(os.listdir(txt_input_dir_path))

        for file_num in range(1, file_count):

            txt_file_path = txt_input_dir_path + 'Test_input' + str(file_num) + '.txt'      # path to 'Test_input0.txt' text file

            txt_input_obj = open(txt_file_path, 'r')

            print('\n============================================')

            print('\nFor Test_input' + str(file_num) + '.txt')

            Digits_list = [int(digit) for digit in txt_input_obj.readline().split()]
            Sum_integer = [int(integer) for integer in txt_input_obj.readline().split()][0]

            print('\nGiven List of Digits = %s \n\nGiven Integer of Sum = %d' % (Digits_list, Sum_integer))

            try:
                
                combination_of_digits = findCombination(Digits_list, Sum_integer)

            except Exception as e:
                
                print('\n[ERROR] findCombination function ran into an error, check your code !\n')
                raise(e)
                exit()
            
            print('\nCombination of digits chosen = %s \n\nNumber of digits chosen = %d' % (combination_of_digits, len(combination_of_digits)))
            
            print('\n============================================')
    
    else:

        print('')


