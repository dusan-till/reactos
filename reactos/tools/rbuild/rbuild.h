#ifndef __RBUILD_H
#define __RBUILD_H

#include "pch.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#define _MAX_PATH 255
#endif
#include <sys/stat.h>
#include <time.h>
#include <utime.h>

#include "ssprintf.h"
#include "exception.h"
#include "XML.h"

#ifdef WIN32
#define EXEPOSTFIX ".exe"
#define CSEP '\\'
#define CBAD_SEP '/'
#define SSEP "\\"
#define SBAD_SEP "/"
#else
#define EXEPOSTFIX ""
#define CSEP '/'
#define CBAD_SEP '\\'
#define SSEP "/"
#define SBAD_SEP "\\"
#endif

class Project;
class Module;
class Include;
class Define;
class File;
class Library;
class Invoke;
class InvokeFile;
class Dependency;
class ImportLibrary;
class If;
class CompilerFlag;
class LinkerFlag;
class Property;
class AutomaticDependency;

class SourceFileTest;

class Project
{
	std::string xmlfile;
	XMLElement *node, *head;
public:
	std::string name;
	std::string makefile;
	std::vector<Module*> modules;
	std::vector<Include*> includes;
	std::vector<Define*> defines;
	std::vector<LinkerFlag*> linkerFlags;
	std::vector<Property*> properties;
	std::vector<If*> ifs;

	//Project ();
	Project ( const std::string& filename );
	~Project ();
	void ProcessXML ( const std::string& path );
	Module* LocateModule ( const std::string& name );
	const Module* LocateModule ( const std::string& name ) const;
private:
	void ReadXml ();
	void ProcessXMLSubElement ( const XMLElement& e,
	                            const std::string& path,
	                            If* pIf = NULL );

	// disable copy semantics
	Project ( const Project& );
	Project& operator = ( const Project& );
};


enum ModuleType
{
	BuildTool,
	StaticLibrary,
	ObjectLibrary,
	Kernel,
	KernelModeDLL,
	KernelModeDriver,
	NativeDLL,
	Win32DLL,
	Win32GUI,
	BootLoader,
	BootSector,
	Iso
};


class Module
{
public:
	const Project& project;
	const XMLElement& node;
	std::string name;
	std::string extension;
	std::string entrypoint;
	std::string path;
	ModuleType type;
	ImportLibrary* importLibrary;
	bool mangledSymbols;
	std::vector<File*> files;
	std::vector<Library*> libraries;
	std::vector<Include*> includes;
	std::vector<Define*> defines;
	std::vector<Invoke*> invocations;
	std::vector<Dependency*> dependencies;
	std::vector<If*> ifs;
	std::vector<CompilerFlag*> compilerFlags;
	std::vector<LinkerFlag*> linkerFlags;

	Module ( const Project& project,
	         const XMLElement& moduleNode,
	         const std::string& modulePath );
	~Module ();
	ModuleType GetModuleType ( const std::string& location,
	                           const XMLAttribute& attribute );
	bool HasImportLibrary () const;
	std::string GetTargetName () const;
	std::string GetDependencyPath () const;
	std::string GetBasePath () const;
	std::string GetPath () const;
	std::string GetPathWithPrefix ( const std::string& prefix ) const;
	std::string GetTargets () const;
	std::string GetInvocationTarget ( const int index ) const;
	bool HasFileWithExtensions ( const std::string& extension1,
	                             const std::string& extension2 ) const;
	void ProcessXML();
private:
	std::string GetDefaultModuleExtension () const;
	std::string GetDefaultModuleEntrypoint () const;
	void ProcessXMLSubElement ( const XMLElement& e,
	                            const std::string& path,
	                            If* pIf = NULL );
};


class Include
{
public:
	const Project& project;
	const Module* module;
	const XMLElement& node;
	std::string directory;
	std::string basePath;

	Include ( const Project& project,
	          const XMLElement& includeNode );
	Include ( const Project& project,
	          const Module* module,
	          const XMLElement& includeNode );
	~Include ();
	void ProcessXML();
private:
	void Initialize();
};


class Define
{
public:
	const Project& project;
	const Module* module;
	const XMLElement& node;
	std::string name;
	std::string value;

	Define ( const Project& project,
	         const XMLElement& defineNode );
	Define ( const Project& project,
	         const Module* module,
	         const XMLElement& defineNode );
	~Define();
	void ProcessXML();
private:
	void Initialize();
};


class File
{
public:
	std::string name;
	bool first;

	File ( const std::string& _name, bool _first );

	void ProcessXML();
};


class Library
{
public:
	const XMLElement& node;
	const Module& module;
	std::string name;

	Library ( const XMLElement& _node,
	          const Module& _module,
	          const std::string& _name );

	void ProcessXML();
};


class Invoke
{
public:
	const XMLElement& node;
	const Module& module;
	const Module* invokeModule;
	std::vector<InvokeFile*> input;
	std::vector<InvokeFile*> output;

	Invoke ( const XMLElement& _node,
	         const Module& _module );

	void ProcessXML();
	std::string GetTargets () const;
private:
	void ProcessXMLSubElement ( const XMLElement& e );
	void ProcessXMLSubElementInput ( const XMLElement& e );
	void ProcessXMLSubElementOutput ( const XMLElement& e );
};


class InvokeFile
{
public:
	const XMLElement& node;
	std::string name;
	std::string switches;

	InvokeFile ( const XMLElement& _node,
	             const std::string& _name );

	void ProcessXML ();
};


class Dependency
{
public:
	const XMLElement& node;
	const Module& module;
	const Module* dependencyModule;

	Dependency ( const XMLElement& _node,
	             const Module& _module );

	void ProcessXML();
};


class ImportLibrary
{
public:
	const XMLElement& node;
	const Module& module;
	std::string basename;
	std::string definition;

	ImportLibrary ( const XMLElement& _node,
	                const Module& module );

	void ProcessXML ();
};


class If
{
public:
	const XMLElement& node;
	const Project& project;
	const Module* module;
	std::string property, value;
	std::vector<File*> files;
	std::vector<Include*> includes;
	std::vector<Define*> defines;
	std::vector<Property*> properties;
	std::vector<If*> ifs;

	If ( const XMLElement& node_,
	     const Project& project_,
	     const Module* module_ );
	~If();

	void ProcessXML();
};


class CompilerFlag
{
public:
	const Project& project;
	const Module* module;
	const XMLElement& node;
	std::string flag;

	CompilerFlag ( const Project& project,
	               const XMLElement& compilerFlagNode );
	CompilerFlag ( const Project& project,
	               const Module* module,
	               const XMLElement& compilerFlagNode );
	~CompilerFlag ();
	void ProcessXML();
private:
	void Initialize();
};


class LinkerFlag
{
public:
	const Project& project;
	const Module* module;
	const XMLElement& node;
	std::string flag;

	LinkerFlag ( const Project& project,
	             const XMLElement& linkerFlagNode );
	LinkerFlag ( const Project& project,
	             const Module* module,
	             const XMLElement& linkerFlagNode );
	~LinkerFlag ();
	void ProcessXML();
private:
	void Initialize();
};


class Property
{
public:
	const XMLElement& node;
	const Project& project;
	const Module* module;
	std::string name, value;

	Property ( const XMLElement& node_,
	           const Project& project_,
	           const Module* module_ );

	void ProcessXML();
};


class SourceFile
{
public:
	SourceFile ( AutomaticDependency* automaticDependency,
	             Module& module,
	             const std::string& filename,
	             SourceFile* parent,
	             bool isNonAutomaticDependency );
	SourceFile* ParseFile ( const std::string& normalizedFilename );
	void Parse ();
	std::string Location () const;
	std::vector<SourceFile*> files;
	AutomaticDependency* automaticDependency;
	Module& module;
	std::string filename;
	std::string filenamePart;
	std::string directoryPart;
	std::vector<SourceFile*> parents; /* List of files, this file is included from */
	bool isNonAutomaticDependency;
	std::string cachedDependencies;
	time_t lastWriteTime;
	time_t youngestLastWriteTime; /* Youngest last write time of this file and all children */
	SourceFile* youngestFile;
private:
	void GetDirectoryAndFilenameParts ();
	void Close ();
	void Open ();
	void SkipWhitespace ();
	bool ReadInclude ( std::string& filename );
	bool IsIncludedFrom ( const std::string& normalizedFilename );
	SourceFile* GetParentSourceFile ();
	bool IsParentOf ( const SourceFile* parent,
	                  const SourceFile* child );
	std::string buf;
	const char *p;
	const char *end;
};


class AutomaticDependency
{
	friend class SourceFileTest;
public:
	const Project& project;

	AutomaticDependency ( const Project& project );
	~AutomaticDependency ();
	void Process ();
	bool LocateIncludedFile ( const std::string& directory,
	                          const std::string& includedFilename,
	                          std::string& resolvedFilename );
	bool LocateIncludedFile ( Module& module,
	                          const std::string& includedFilename,
	                          std::string& resolvedFilename );
	SourceFile* RetrieveFromCacheOrParse ( Module& module,
	                                       const std::string& filename,
	                                       SourceFile* parentSourceFile );
	SourceFile* RetrieveFromCache ( const std::string& filename );
	void CheckAutomaticDependencies ();
	void CheckAutomaticDependenciesForFile ( SourceFile* sourceFile );
private:
	void ProcessModule ( Module& module );
	void ProcessFile ( Module& module,
	                   const File& file );
	std::map<std::string, SourceFile*> sourcefile_map;
};


extern std::string
FixSeparator ( const std::string& s );

extern std::string
GetExtension ( const std::string& filename );

extern std::string
NormalizeFilename ( const std::string& filename );

#endif /* __RBUILD_H */
