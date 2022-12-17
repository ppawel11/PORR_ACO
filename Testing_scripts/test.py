import xlsxwriter
import os
import subprocess

workbook = xlsxwriter.Workbook('full_results.xlsx')
worksheet16 = workbook.add_worksheet("16_ants")
worksheet32 = workbook.add_worksheet("32_ants")
worksheet64 = workbook.add_worksheet("64_ants")

worksheet16.write(0, 0, "Basic")
worksheet16.write(0, 1, "4 threads")
worksheet16.write(0, 2, "8 threads")
worksheet16.write(0, 3, "16 threads")

worksheet32.write(0, 0, "Basic")
worksheet32.write(0, 1, "4 threads")
worksheet32.write(0, 2, "8 threads")
worksheet32.write(0, 3, "16 threads")

worksheet64.write(0, 0, "Basic")
worksheet64.write(0, 1, "4 threads")
worksheet64.write(0, 2, "8 threads")
worksheet64.write(0, 3, "16 threads")

row = 1

for i in range(10):
	process = os.popen("cd ../cmake-build-debug; ./Projekt basic 16")
	worksheet16.write(row, 0, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 16 4")
	worksheet16.write(row, 1, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 16 8")
	worksheet16.write(row, 2, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 16 16")
	worksheet16.write(row, 3, process.read())
	
	process = os.popen("cd ../cmake-build-debug; ./Projekt basic 32")
	worksheet32.write(row, 0, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 32 4")
	worksheet32.write(row, 1, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 32 8")
	worksheet32.write(row, 2, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 32 16")
	worksheet32.write(row, 3, process.read())
	
	process = os.popen("cd ../cmake-build-debug; ./Projekt basic 64")
	worksheet64.write(row, 0, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 64 4")
	worksheet64.write(row, 1, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 64 8")
	worksheet64.write(row, 2, process.read())
	process = os.popen("cd ../cmake-build-debug; ./Projekt omp 64 16")
	worksheet64.write(row, 3, process.read())
	
	print("Finished row")
	row += 1

workbook.close()


