//created by Ziyad Barakat 2015

#ifndef TINY_SHADERS_H
#define TINY_SHADERS_H

#if defined(_WIN32) || defined(_WIN64)
#define TS_WINDOWS
#include <Windows.h>
#include <gl/GL.h>
//disable annoying warnings about unsafe stdio functions
#pragma  warning(disable: 4474)
#pragma  warning(disable: 4996)
//this automatically loads the OpenGL library if you are using Visual studio 
//comment this out if you have your own method 
//#pragma comment (lib, "opengl32.lib")
#endif

#if defined(__linux__) 
#include <GL/gl.h>
#endif

#include <list>
#include <vector>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <memory>
#include <system_error>
#include <bitset>
#include <sys/stat.h>

namespace TinyShaders
{
	enum class error_t
	{
		success,
		invalidShaderProgramName,
		invalidShaderProgramIndex,
		invalidShaderName,
		invalidShaderIndex,
		invalidFilePath,
		shaderNotFound,
		shaderProgramNotFound,
		invalidShaderType,
		shaderLoadFailed,
		shaderProgramLoadFailed,
		shaderCompileFailed,
		shaderProgramcompileFailed,
		shaderProgramLinkFailed,
		shaderAlreadyExists,
		shaderAlreadyCompiled,
		shaderProgramAlreadyExists,
		shaderProgramAlreasyCompiled,
		invalidConfigFile,
		invalidSourceFiles,
		invalidBuffer,
	};

	class errorCategory_t : public std::error_category
	{
	public:
		const char* name() const throw() override
		{
			return "tinyShaders";
		}

		/**
		* return the error number associated with the given error number
		*/
		virtual std::string message(int errorValue) const override
		{
			error_t err = (error_t)errorValue;
			switch (err)
			{
			case error_t::success:
			{
				return "function call was successful \n";
			}

			case  error_t::invalidShaderProgramName:
			{
				return "Error: invalid shader program name \n";
			}

			case error_t::invalidShaderProgramIndex:
			{
				return "Error: invalid shader program number \n";
			}

			case error_t::invalidShaderName:
			{
				return "Error: invalid shader name \n";
			}

			case error_t::invalidShaderIndex:
			{
				return "Error: invalid shader index \n";
			}

			case error_t::invalidFilePath:
			{
				return "Error: invalid file path \n";
			}

			case error_t::shaderNotFound:
			{
				return "Error: shader not found \n";
			}

			case error_t::shaderProgramNotFound:
			{
				return "Error: shader program not found \n";
			}

			case error_t::invalidShaderType:
			{
				return "Error: invalid shader type \n";
			}

			case error_t::shaderLoadFailed:
			{
				return "Error: shader load failed \n";
			}

			case error_t::shaderProgramLoadFailed:
			{
				return "Error: shader program load failed \n";
			}

			case error_t::shaderProgramLinkFailed:
			{
				return "Error: shader program link failed \n";
			}

			case error_t::shaderAlreadyExists:
			{
				return "Error: shader already exists \n";
			}

			case error_t::shaderAlreadyCompiled:
			{
				return "Error: shader already compiled \n";
			}

			case error_t::shaderProgramAlreadyExists:
			{
				return "Error: shader program already exists \n";
			}

			case error_t::shaderProgramAlreasyCompiled:
			{
				return "Error: shader program already compiled \n";
			}

			case error_t::invalidSourceFiles:
			{
				return "Error: invalid source files \n";
			}

			case error_t::invalidBuffer:
			{
				return "Error: invalid buffer \n";
			}

			default:
			{
				return "Error: unspecified error \n";
			}
			}
		}

		errorCategory_t() {};

		const static errorCategory_t& get()
		{
			const static errorCategory_t category;
			return category;
		}
	};

	inline std::error_code make_error_code(error_t errorCode)
	{
		return std::error_code(static_cast<int>(errorCode), errorCategory_t::get());
	}
};

namespace std
{
	template<> struct is_error_code_enum<TinyShaders::error_t> : std::true_type {};
};

namespace TinyShaders
{
	//the macros below are safe for editing
	std::string defaultProrgamBinaryExtension = ".glbin";
	std::string defaultBinaryPath = "./Shaders/";
	std::string defaultBinaryConfig = "Binaries.txt";
	std::string defaultProgramPath = "Shaders.txt";

	unsigned int maxNumShaderComponents = 5;								/**< The Maximum number of components a shader program can have. It's always 5*/

	typedef std::function<void(GLuint programHandle)>		parseBlocks_t;	/**< a callback that can gather all the info about the uniform blocks that are in a shader program*/

	/*
	* a shader_t is essentially an OpenGL shader
	*/
	struct shader_t
	{
		shader_t(const GLchar* shaderName, GLuint shaderType, const GLchar* shaderFilePath) :
			name(shaderName)
		{
			type = shaderType;
			isCompiled = GL_FALSE;
			filePath = shaderFilePath;
			FileToBuffer(shaderFilePath, buffer);

			Compile(buffer);
		}

		shader_t(const GLchar* shaderName, std::string buffer, GLuint shaderType)
			: name(shaderName), type(shaderType), buffer(buffer)
		{
			type = shaderType;
			isCompiled = false;
			Compile(buffer);
			filePath = NULL;

		}
		shader_t()
		{
			name = NULL;
			handle = 0;
			type = 0;
			isCompiled = false;;
			filePath = NULL;
		}
		~shader_t() {}

		/*
		* compile the shader from a given text file
		*/
		std::error_code Compile(std::string source)
		{
			//if the component hasn't been compiled yet
			if (!isCompiled)
			{
				GLchar errorLog[512];
				GLint successful;

				if (!source.empty())
				{
					handle = glCreateShader(type);
					glShaderSource(handle, 1, (const GLchar**)&source, 0);
					glCompileShader(handle);

					glGetShaderiv(handle, gl_compile_status, &successful);
					glGetShaderInfoLog(handle, sizeof(errorLog), 0, errorLog);

					if (!successful)
					{
						return error_t::shaderLoadFailed;
					}

					else
					{
						isCompiled = true;
						return error_t::success;
					}
				}
				else
				{
					return error_t::invalidSourceFiles;
				}
			}
			else
			{
				//the component has already been loaded
				return error_t::shaderAlreadyCompiled;
			}
		}

		/*
		* remove the shader from OpenGL
		*/
		void Shutdown()
		{
			glDeleteShader(handle);
			isCompiled = GL_FALSE;
		}

		/*
		* convert the given file to a single dimension c-string buffer
		*/
		std::error_code FileToBuffer(const GLchar* path, std::string& outBuffer) const
		{
			FILE* file = fopen(path, "rt");

			if (file == nullptr)
			{
				return error_t::invalidFilePath;
			}

			struct stat fileSize;
			size_t FileLength = 0;

			if (!stat(path, &fileSize))
			{
				FileLength = fileSize.st_size;
			}

			else
			{
				return error_t::invalidFilePath;
			}

			//get total byte in given file
			/*fseek(file, 0, SEEK_END);
			size_t FileLength = ftell(file);
			fseek(file, 0, SEEK_SET);*/

			//allocate a file buffer and read the contents of the file
			GLchar* buffer = new GLchar[FileLength + 1];
			memset(buffer, 0, FileLength + 1);
			fread(buffer, sizeof(GLchar), FileLength, file);

			fclose(file);
			outBuffer = buffer;
			delete buffer;
			return error_t::success;
		}

		const GLchar*		name;			/**<The name of the shader component */
		const GLchar*		filePath;		/**<The FilePath of the component*/
		GLuint				handle;			/**<The handle to the shader in OpenGL*/
		GLuint				type;			/**<The type of shader ( Vertex, Fragment, etc.)*/
		GLboolean			isCompiled;		/**<Whether the shader has been compiled*/
		std::string			buffer;			/**<the source code of the shader*/
	};

	/*
	* a shaderProgram_t is is essentially an OpengL shader program
	*/
	struct shaderProgram_t
	{
		shaderProgram_t()
		{
			compiled = false;
			name = 0;
			handle = 0;
		};

		/*
		* uses the given values to create an OpenGL shader program
		*/
		shaderProgram_t(const GLchar* programName,
			std::vector< std::string > programInputs,
			std::vector< std::string > programOutputs,
			std::vector< std::unique_ptr<shader_t>> programShaders,
			bool saveBinary = false) :
			name(programName), inputs(programInputs),
			outputs(programOutputs), shaders(std::move(programShaders))
		{
			compiled = GL_FALSE;
			Compile(saveBinary);
		};

		/*
		* another bare bones constructor
		*/
		shaderProgram_t(const GLchar* programName) : name(programName)
		{
			handle = 0;
			compiled = false;
		};

		shaderProgram_t(const GLchar* programName, GLuint programHandle) :
			name(programName), handle(programHandle)
		{
			compiled = false;
		}

		~shaderProgram_t() {}

		/*
		* shut down the shader program. delete it from OpenGL
		*/
		void Shutdown()
		{
			glDeleteProgram(handle);

			//clear up all the shaders
			for (size_t iterator = 0; iterator < shaders.size(); iterator++)
			{
				shaders[iterator]->Shutdown();
			}
			shaders.clear();
			inputs.clear();
			outputs.clear();
		}

		/*
		* compile the OpenGL shader program with the given information
		*/
		std::error_code Compile(bool saveBinary)
		{
			handle = glCreateProgram();
			GLchar errorLog[512];
			GLint successful = false;
			if (!compiled)
			{
				for (size_t iterator = 0; iterator < shaders.size(); iterator++)
				{
					if (shaders[iterator] != nullptr)
					{
						glAttachShader(handle, shaders[iterator]->handle);
					}
				}

				// specify vertex input attributes
				for (size_t i = 0; i < inputs.size(); ++i)
				{
					glBindAttribLocation(handle, (GLuint)i, inputs[i].c_str());
				}

				// specify pixel shader outputs
				for (size_t i = 0; i < outputs.size(); ++i)
				{
					glBindFragDataLocation(handle, (GLuint)i, outputs[i].c_str());
				}

				if (saveBinary)
				{
					glProgramParameteri(handle, gl_program_binary_retrievable_hint, GL_TRUE);
				}

				glLinkProgram(handle);
				glGetProgramiv(handle, gl_link_status, &successful);

				if (!successful)
				{
					glGetProgramInfoLog(handle, sizeof(errorLog), 0, errorLog);
					return error_t::shaderProgramLinkFailed;
				}

				//if a shader successfully compiles then it will add itself to storage
				if (saveBinary)
				{
					GLsizei binarySize = 0;
					glGetProgramiv(handle, gl_program_binary_length, &binarySize);

					void* buffer = (void*)malloc((size_t)binarySize);
					GLenum binaryFormat = NULL;

					glGetProgramBinary(handle, binarySize, NULL, &binaryFormat, buffer);

					GLchar* path = new GLchar[(size_t)binarySize];
					memset(path, 1, (size_t)binarySize);

					strcpy(path, defaultBinaryPath.c_str());
					strcat(path, name);
					strcat(path, defaultProrgamBinaryExtension.c_str());

					FILE* file = fopen(path, "wb");
					fprintf(file, "%s\n", name);
					fprintf(file, "%i\n", binarySize);
					fprintf(file, "%i\n", binaryFormat);
					fwrite(buffer, (size_t)binarySize, 1, file);
					fclose(file);
					delete[] path;
				}
				compiled = true;
				return error_t::success;
			}
			return error_t::shaderProgramAlreasyCompiled;
		}

		const GLchar*										name;				/**< The name of the shader program */
		GLuint												handle;				/**< The OpenGL handle to the shader program */
		GLboolean											compiled;			/**< Whether the shader program has been linked successfully */
		std::vector< std::string >							inputs;				/**< The inputs of the shader program as a vector of strings */
		std::vector< std::string >							outputs;			/**< The outputs of the shader program as a vector of strings */
		std::vector< std::unique_ptr<shader_t> >			shaders;			/**< The components that the shader program is comprised of as a vector */
	};

	class shaderManager
	{
	public:

		std::map<std::string, std::unique_ptr<shaderProgram_t>>			shaderPrograms;		/**< All loaded shader programs */
		std::map<std::string, std::unique_ptr<shader_t>>				shaders;			/**< All loaded shaders*/

		shaderManager() {}
		~shaderManager() {}

		/*
		* shuts down TinyShaders. deletes all OpenGL shaders and shader programs
		* as well as calling shutdown on all shader and programs and clears all vectors.
		*/
		void Shutdown()
		{

			for (auto iter = shaders.begin(); iter != shaders.end(); iter++)
			{	
				iter->second->Shutdown();
				iter->second.release();
			}
			for (auto iter = shaderPrograms.begin(); iter != shaderPrograms.end(); iter++)
			{
				
				iter->second->Shutdown();
				iter->second.release();
			}

			

			shaderPrograms.clear();
			shaders.clear();
		}

		/*
		* load an OpenGL shader
		*/
		std::error_code LoadShader(const GLchar* name, shader_t* outShader, const GLchar* shaderFile, GLuint shaderType)
		{
			if (name != nullptr)
			{
				if (shaderType <= 5)
				{
					shader_t* newShader = new shader_t(name, shaderType, shaderFile);
					if (newShader->isCompiled)
					{
						shaders.insert(std::make_pair(name, std::move(newShader)));
						outShader = shaders[name].get();
						return error_t::success;
					}
					return error_t::shaderCompileFailed;
				}
				return error_t::invalidShaderType;
			}
			return error_t::invalidShaderName;
		}

		/*
		* loads all shaders and shader programs specified in a custom configuration file
		*/
		std::error_code LoadShaderProgramsFromConfigFile(const GLchar* configPath, std::vector<shaderProgram_t*>& outPrograms, bool saveBinary = false)
		{
			FILE* pConfigFile = fopen(configPath, "r");
			GLuint numInputs = 0;
			GLuint numOutputs = 0;
			GLuint numPrograms = 0;
			GLuint numShaders = 0;
			GLuint iterator = 0;

			std::vector< std::string > inputs, outputs, paths, names;
			std::vector< std::unique_ptr<shader_t>> newShaders;
			if (pConfigFile)
			{
				//get the total number of shader programs
				fscanf(pConfigFile, "%i\n", &numPrograms);

				for (GLuint programIter = 0;
					programIter < numPrograms;
					programIter++, paths.clear(), inputs.clear(), outputs.clear(), names.clear(), newShaders.clear())
				{
					//get the name of the shader program 
					GLchar* programName = new GLchar[255];
					fscanf(pConfigFile, "%s\n", programName);

					//this is an anti-trolling measure. If a shader with the same name already exists the don't bother making a new one.
					if (shaderPrograms.find(programName) == shaderPrograms.end())
					{
						//get the number of shader inputs
						fscanf(pConfigFile, "%i\n", &numInputs);

						//get all inputs
						for (iterator = 0; iterator < numInputs; iterator++)
						{
							GLchar* input = new GLchar[255];
							fscanf(pConfigFile, "%s\n", input);
							inputs.push_back(input);
						}

						//get the number of shader outputs
						fscanf(pConfigFile, "%i\n", &numOutputs);

						//get all outputs
						for (iterator = 0; iterator < numOutputs; iterator++)
						{
							GLchar* output = new GLchar[255];
							fscanf(pConfigFile, "%s\n", output);
							outputs.push_back(output);
						}

						//get number of shaders
						fscanf(pConfigFile, "%i\n", &numShaders);

						for (GLuint iterator = 0; iterator < numShaders; iterator++)
						{
							GLchar* shaderName = new GLchar[255];
							GLchar* shaderPath = new GLchar[255];
							GLchar* shaderType = new GLchar[255];

							//get shader name
							fscanf(pConfigFile, "%s\n", shaderName);

							//if the shader hasn't been loaded already then make a new one
							if (shaders.find(shaderName) == shaders.end())
							{
								//get type
								fscanf(pConfigFile, "%s\n", shaderType);
								//get file path
								fscanf(pConfigFile, "%s\n", shaderPath);

								shader_t* newShader = new shader_t(shaderName, StringToShaderType((const char*)shaderType), shaderPath);
								if (newShader->isCompiled)
								{
									shaders.insert(std::make_pair(shaderName, std::move(newShader)));
									newShaders.push_back(std::move(shaders[shaderName]));
								}
							}

							else
							{
								//tell scanf to skip a couple lines
								fscanf(pConfigFile, "%*[^\n]\n %*[^\n]\n");
								//if shader already exists then add an existing one from storage, it should already be compiled
								newShaders.push_back(std::move(shaders[shaderName]));
							}
						}

						shaderProgram_t* newShaderProgram = new shaderProgram_t(programName, inputs, outputs, std::move(newShaders), saveBinary);
						if (newShaderProgram->compiled)
						{
							shaderPrograms.insert(std::make_pair(programName, std::move(newShaderProgram)));
							outPrograms.push_back(shaderPrograms[programName].get());
						}
					}
				}
				fclose(pConfigFile);
				return error_t::success;
			}
			else
			{
				return error_t::invalidConfigFile;
			}
		}

		/*
		* loads shader program binaries using a config file
		*/
		std::error_code LoadProgramBinariesFromConfigFile(const GLchar* configPath, std::vector<shaderProgram_t*>& outPrograms)
		{
			//open a file stream to binaries.txt
			GLuint numBinaries;
			FILE* configFile = nullptr;
			if (configPath != nullptr)
			{
				configFile = fopen(configPath, "r");
				if (configFile != nullptr)
				{
					fscanf(configFile, "%i", &numBinaries);
					fscanf(configFile, "%*[^\n]\n %*[^\n]\n"); //skip a couple lines
					for (unsigned int iter = 0; iter < numBinaries; iter++)
					{
						GLchar binaryPath[255];
						fscanf(configFile, "%s \n", binaryPath);

						FILE* binaryFile = fopen(binaryPath, "rb");
						//std::ifstream file;
						GLchar binaryName[255];
						GLuint binarySize = 0;
						GLuint size = 0;
						GLuint binaryFormat = 0;

						fscanf(binaryFile, "%s \n", binaryName);
						fscanf(binaryFile, "%i \n", &binarySize);
						fscanf(binaryFile, "%i \n", &binaryFormat);

						void* binaryBuffer = (void*)malloc((size_t)binarySize);
						fread(binaryBuffer, (size_t)binarySize, 1, binaryFile);
						fclose(binaryFile);

						//load the buffer into OpenGL
						GLuint programHandle = glCreateProgram();
						glProgramBinary(programHandle, binaryFormat, binaryBuffer, binarySize);
						free(binaryBuffer);
						GLint isSuccessful = false;

						glGetProgramiv(programHandle, gl_link_status, &isSuccessful);

						if (isSuccessful)
						{
							//create a program object and load into the list
							std::unique_ptr<shaderProgram_t> newProgram(new shaderProgram_t(binaryName, programHandle));
							if (newProgram->compiled)
							{
								//only if its successful. release the data if it isnt? or does it remove itself when it goes out of scope?
								shaderPrograms.insert(std::make_pair(binaryName, std::move(newProgram)));
								outPrograms.push_back(shaderPrograms[binaryName].get());
								return error_t::success;
							}

							else
							{
								shaderProgram_t* toDelete = newProgram.release();
								delete toDelete;
								return error_t::shaderProgramLoadFailed;
							}
						}
						return error_t::shaderProgramLoadFailed;
					}
					fclose(configFile);
				}

				else
				{
					return error_t::invalidConfigFile;
				}
			}

			else
			{
				return error_t::invalidFilePath;
			}
		}

		/*
		* loads shaders from a config file
		*/
		std::error_code LoadShadersFromConfigFile(const GLchar* configFile, std::vector<shader_t*>& outShaders)
		{
			FILE* pConfigFile = fopen(configFile, "r+");
			int numShaders = 0;

			if (pConfigFile)
			{
				//get the number of shaders to load
				fscanf(pConfigFile, "%i\n", &numShaders);
				GLchar* shaderName;
				GLchar*	shaderType;
				GLchar*	shaderPath;

				GLchar empty[255];

				for (int iterator = 0; iterator < numShaders;
					iterator++, fscanf(pConfigFile, "\n\n"))
				{
					shaderName = empty;
					fscanf(pConfigFile, "%s\n", shaderName);

					//if it doesn't find an existing shader with a matching name then add it. else ignore it
					if (shaders.find(shaderName) == shaders.end())
					{
						shaderType = empty;
						fscanf(pConfigFile, "%s\n", shaderType);

						shaderPath = empty;
						fscanf(pConfigFile, "%s\n", shaderPath);

						shader_t* newShader = new shader_t(shaderName, StringToShaderType(shaderType), shaderPath);
						if (newShader->isCompiled)
						{
							shaders.insert(std::make_pair(shaderName, std::move(newShader)));
							return error_t::success;
						}
					}
				}
				return error_t::shaderNotFound;
			}

			else
			{
				return error_t::invalidConfigFile;
			}
		}

		/*
		* saves all loaded shaders and shader programs to a config file
		*/
		std::error_code SaveShaderProgramsToConfigFile(const GLchar* fileName)
		{
			//write total amount of shaders
			FILE* pConfigFile = fopen(fileName, "w+");

			if (pConfigFile)
			{
				fprintf(pConfigFile, "%i\n\n", (GLint)shaderPrograms.size());

				for (auto iter = shaderPrograms.begin(); iter != shaderPrograms.end(); iter++)
				{
					//write program name
					fprintf(pConfigFile, "%s\n", iter->second.get()->name);

					//write number of inputs
					fprintf(pConfigFile, "%i\n", (GLint)iter->second.get()->inputs.size());

					//write inputs
					for (size_t inputIter = 0; inputIter < iter->second.get()->inputs.size(); inputIter++)
					{
						fprintf(pConfigFile, "%s\n", iter->second.get()->inputs[inputIter].c_str());
					}

					fprintf(pConfigFile, "%i\n", (GLint)iter->second.get()->outputs.size());

					//write outputs
					for (size_t outputIter = 0; outputIter < iter->second.get()->outputs.size(); outputIter++)
					{
						fprintf(pConfigFile, "%s\n", iter->second.get()->outputs[outputIter].c_str());
					}

					//write number of shaders
					fprintf(pConfigFile, "%i\n", (GLint)iter->second.get()->shaders.size());

					for (size_t shaderIter = 0; shaderIter < iter->second.get()->shaders.size(); shaderIter++)
					{
						//write shader name
						fprintf(pConfigFile, "%s\n", iter->second.get()->shaders[shaderIter]->name);

						//write shader type
						fprintf(pConfigFile, "%s\n", ShaderTypeToString(iter->second.get()->shaders[shaderIter]->type));

						//write shader file path
						fprintf(pConfigFile, "%s\n", iter->second.get()->shaders[shaderIter]->filePath);
					}
				}
				fclose(pConfigFile);
				return error_t::success;
			}

			else
			{
				return error_t::invalidConfigFile;
			}
		}

		/*
		* builds a new OpenGL shader program from already loaded shaders
		*/
		std::error_code BuildProgramFromShaders(const GLchar* shaderName,
			const std::vector< std::string >& inputs,
			const std::vector< std::string >& outputs,
			const GLchar* vertexShaderName,
			const GLchar* fragmentShaderName,
			const GLchar* geometryShaderName,
			const GLchar* tessContShaderName,
			const GLchar* tessEvalShaderName,
			bool saveBinary = false)
		{
				std::vector< std::unique_ptr<shader_t>> reusedShaders;
				reusedShaders.push_back(std::move(shaders[vertexShaderName]));
				reusedShaders.push_back(std::move(shaders[fragmentShaderName]));
				reusedShaders.push_back(std::move(shaders[geometryShaderName]));
				reusedShaders.push_back(std::move(shaders[tessContShaderName]));
				reusedShaders.push_back(std::move(shaders[tessEvalShaderName]));

				std::unique_ptr<shaderProgram_t> newShaderProgram(new shaderProgram_t(shaderName, inputs, outputs, std::move(reusedShaders), saveBinary));
				if (newShaderProgram.get()->compiled)
				{
					shaderPrograms.insert(std::make_pair(shaderName, std::move(newShaderProgram)));
				}

				return error_t::success;
		}

		/*
		* loads an OpengL shader from char buffer
		*/
		std::error_code LoadShaderFromBuffer(const char* name, const GLchar* buffer, GLuint shaderType)
		{
			if (buffer != nullptr)
			{
				if (name != nullptr)
				{
					//if the shader doesn't already exist the add it. else ignore it
					if (shaders.find(name) == shaders.end())
					{
						shader_t* newShader = new shader_t(name, buffer, shaderType);
						if (newShader->isCompiled)
						{
							shaders.insert(std::make_pair(name, std::move(newShader)));
						}
						return error_t::success;
					}
					return error_t::shaderAlreadyExists;
				}
				return error_t::invalidShaderName;
			}
			return error_t::invalidBuffer;
		}

		//not sure what to do with this for the time being. just gonna leave it
		void SetShaderBlockParseEvent(parseBlocks_t shaderBlockParse)
		{
			shaderBlocksEvent = shaderBlockParse;
		}

	private:

		/*
		* convert the given string to a shader type
		*/
		GLuint StringToShaderType(const GLchar* typeString) const
		{
			if (typeString != nullptr)
			{
				if (!strcmp(typeString, "Vertex"))
				{
					return gl_vertex_shader;
				}

				if (!strcmp(typeString, "Fragment"))
				{
					return gl_fragment_shader;
				}

				if (!strcmp(typeString, "Geometry"))
				{
					return gl_geometry_shader;
				}

				if (!strcmp(typeString, "Tessellation Control"))
				{
					return gl_tess_control_shader;
				}

				if (!strcmp(typeString, "Tessellation Evaluation"))
				{
					return gl_tess_evaluation_shader;
				}

				return GL_FALSE;
			}
			return GL_FALSE;
		}

		/*
		* convert the given shader type to a string
		*/
		const GLchar* ShaderTypeToString(GLuint shaderType) const
		{
			switch (shaderType)
			{
			case gl_vertex_shader:
			{
				return "Vertex";
			}

			case gl_fragment_shader:
			{
				return "Fragment";
			}

			case gl_geometry_shader:
			{
				return "Geometry";
			}

			case gl_tess_control_shader:
			{
				return "Tessellation Control";
			}

			case gl_tess_evaluation_shader:
			{
				return "Tessellation Evaluation";
			}

			default:
			{
				return NULL;
			}
			}

			return nullptr;
		}

		parseBlocks_t									shaderBlocksEvent;
	};
}
#endif
