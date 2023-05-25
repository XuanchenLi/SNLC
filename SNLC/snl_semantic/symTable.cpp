#include "symTable.h"

const char* praseTypeKind(TypeKind kind) {
	switch (kind) {
	case intTy:return "int"; break;
	case charTy:return "char"; break;
	case boolTy:return "bool"; break;
	case arrayTy:return "array"; break;
	case recordTy:return "record"; break;
	}
}

const char* praseIdKind(IdKind kind) {
	switch (kind) {

	case varKind:return "var"; break;
	case procKind:return "proc"; break;
	case typeKind:return "type"; break;

	}
}
const char* praseAccess(AccessKind kind) {
	switch (kind) {

	case indir:return "indir"; break;
	case didr:return "didr"; break;

	}
}