/*
 * fileManager.h
 *
 *  Created on: 12/09/2011
 *      Author: Mariano Jiménez Brenes
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <stdio.h>
#include <iostream>
#include <string>

class fileManager
{
public:
	fileManager();

	~fileManager();

	void initFile(char* nombre);

	void writeFile(int blockSize, float* in, float* out);

	void writeln(int blockSize, float* in);

	void closeFile();

private:
	FILE* archivo_;
protected:

};

#endif /* FILEMANAGER_H_ */
