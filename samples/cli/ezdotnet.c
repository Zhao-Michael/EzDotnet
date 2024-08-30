/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 **/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "common/common.h"

#ifndef DEBUG_MODE
#define DEBUG_MODE 0
#endif

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

typedef ASMHANDLE(APICALL* clrInitFunc)(const char* asmPath, const char* asmDir, int enableDebug);
typedef int (APICALL* clrDeInit)(ASMHANDLE handle);
typedef int (APICALL* runMethodFunc)(ASMHANDLE handle, const char* typeName, const char* methodName, int argc, char* argv[]);

#if defined(WIN32) || defined(__CYGWIN__)
#include <Windows.h>
#define GET_PWD(buf, size) GetCurrentDirectory(size, buf)
#else
#include <unistd.h>
#define GET_PWD(buf, size) getcwd(buf, size)
#endif


EXPORT int initdotnet(const char* loaderPath, const char* asmPath, ASMHANDLE* p_ASMHANDLE, runMethodFunc* p_runMethod, clrDeInit* p_clrDeInit) {
	char* finalLoaderPath = NULL;

#ifdef __CYGWIN__
	initCygwin();
	finalLoaderPath = to_windows_path(loaderPath);
#else
	finalLoaderPath = (char*)loaderPath;
#endif

	void* hmod = LIB_OPEN(finalLoaderPath);

#ifdef __CYGWIN__
	free(finalLoaderPath);
#endif

	if (hmod == NULL) {
		fprintf(stderr, "Failed to load %s\n", finalLoaderPath);
		return -1;
	}

	fprintf(stderr, "Handle: %p\n", hmod);
	clrInitFunc clrInit = (clrInitFunc)LIB_GETSYM(hmod, "clrInit");
	if (clrInit == NULL) {
		fputs("clrInit not found", stderr);
		return -1;
	}

	runMethodFunc runMethod = (runMethodFunc)LIB_GETSYM(hmod, "runMethod");
	if (runMethod == NULL) {
		fputs("runMethod not found", stderr);
		return -1;
	}

	clrDeInit clrDeInit = (runMethodFunc)LIB_GETSYM(hmod, "clrDeInit");
	if (runMethod == NULL) {
		fputs("clrDeInit not found", stderr);
		return -1;
	}

	char buf[255];
	GET_PWD(buf, sizeof(buf));

	printf("calling clrInit, pwd: %s, asm: %s\n", buf, asmPath);
	ASMHANDLE handle = clrInit(asmPath, buf, DEBUG_MODE);

	*p_runMethod = runMethod;
	*p_ASMHANDLE = handle;
	*p_clrDeInit = clrDeInit;
	return 0;
}


EXPORT int rundotnet(
	runMethodFunc p_runMethod,
	ASMHANDLE p_ASMHANDLE,
	const char* targetClassName,
	const char* targetMethodName,
	int argc,
	const char* argv[]
) {
	printf("calling runMethod, handle: %zu\n", p_ASMHANDLE);
	return p_runMethod(p_ASMHANDLE, targetClassName, targetMethodName, argc, argv);
}

int main(int argc, char* argv[]) {
	if (argc < 5) {
		fprintf(stderr, "Usage: %s [loaderPath] [asmPath] [className] [methodName]\n", argv[0]);
		return 1;
	}
	const char* loaderPath = argv[1];
	const char* asmPath = argv[2];
	const char* className = argv[3];
	const char* methodName = argv[4];
	int mod_argc = argc - 5;
	char** mod_argv = (char**)&argv[5];

	ASMHANDLE p_ASMHANDLE = 0;
	runMethodFunc p_runMethod = 0;
	clrDeInit p_clrDeInit = 0;
	int res = initdotnet(loaderPath, asmPath, &p_ASMHANDLE, &p_runMethod, &p_clrDeInit);
	rundotnet(p_runMethod, p_ASMHANDLE, className, methodName, mod_argc, mod_argv);
	p_clrDeInit(p_ASMHANDLE);
	return res;
}
