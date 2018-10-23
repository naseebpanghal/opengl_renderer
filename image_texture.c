
/*global vertices */
static GLfloat vVertices[] = { 
	-1.0f,  1.0f, 0.0f,  // Position 0
	0.0f,  0.0f,        // TexCoord 0
	-1.0f, -1.0f, 0.0f,  // Position 1
	0.0f,  1.0f,        // TexCoord 1
	1.0f, -1.0f, 0.0f,  // Position 2
	1.0f,  1.0f,        // TexCoord 2
	1.0f,  1.0f, 0.0f,  // Position 3
	1.0f,  0.0f         // TexCoord 3
};
static GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

static GLfloat vTex90[] = { 
	0.0f,  1.0f,        // TexCoord 1
    1.0f,  1.0f,        // TexCoord 2
    1.0f,  0.0f,         // TexCoord 3
	0.0f,  0.0f,        // TexCoord 0  
};
static GLfloat vTex180[] = { 
	1.0f,  1.0f,        // TexCoord 2
    1.0f,  0.0f,         // TexCoord 3
	0.0f,  0.0f,        // TexCoord 0
    0.0f,  1.0f,        // TexCoord 1
};
static GLfloat vTex270[] = { 
	1.0f,  0.0f,         // TexCoord 3
	0.0f,  0.0f,        // TexCoord 0
    0.0f,  1.0f,        // TexCoord 1
    1.0f,  1.0f,        // TexCoord 2
};
static GLfloat vTexInverted[] = { 
    0.0f,  1.0f,        // TexCoord 1
    0.0f,  0.0f,        // TexCoord 0
    1.0f,  0.0f,         // TexCoord 3
    1.0f,  1.0f,        // TexCoord 2
};


image_texture_init(int aWidth, int aHeight, int internalGlDataType)
{
#if (defined(__WIN32))
	glewExperimental=GL_TRUE;
	GLenum err = glewInit(); 
	if (err != GLEW_OK)
	{
		printf("glew init failed\n");
	}
#endif
	GLenum text;
	m_texture = 0;
	m_videoFormat = -1;
	m_rotation = -1;
	const GLubyte* glVersion = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	//easy_printf("glVersion:%s, glslVersion:%s\n", glVersion, glslVersion);
	m_shader_vp = glCreateShader(GL_VERTEX_SHADER);
	m_shader_fp = glCreateShader(GL_FRAGMENT_SHADER);
	m_shaderNV12_fp = glCreateShader(GL_FRAGMENT_SHADER);
	m_shaderRGB_fp = glCreateShader(GL_FRAGMENT_SHADER);

	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		//printf("glCreateShader failed with error: %d.\n", text);
	}
	const char *vsText =  "attribute vec4 a_position;   \n"
		"attribute vec2 a_texCoord;   \n"
		"varying vec2 v_texCoord;     \n"
		"void main()                  \n"
		"{                            \n"
		"   gl_Position = a_position; \n"
		"   v_texCoord = a_texCoord;  \n"
		"}                            \n";//textFileRead(vsFile);
	const char *fsText = 
			//precision directive has to be added if supported version is 1.0. Version 1.10 of the language does not require shaders to include this directive(in fedora it's seems it's needed)
			//but i had prob compiling the shader on some pc with default precision, read somewhere that in some systems of non-ES should not add default precision(so precision was commented in prev versions)
			//"#version 100                                   \n"
			//"#version 300 es                                   \n"
			"precision mediump float;                            \n"
			"varying vec2 v_texCoord;                            \n"
			"uniform sampler2D s_Y;                       		 \n"
			"uniform sampler2D s_U;                       		 \n"
			"uniform sampler2D s_V;                       		 \n"
			"void main()                                         \n"
			"{                                                   \n"
			"  float Y1;                                 		 \n"
			"  float U1;                                 		 \n"
			"  float V1;                                 		 \n"
			"                                                    \n"
			"  Y1 = texture2D( s_Y, v_texCoord ).x - 0.0625;  	 \n"
			"  U1 = texture2D( s_U, v_texCoord ).x - 0.5; 		 \n"
			"  V1 = texture2D( s_V, v_texCoord ).x - 0.5;		 \n"
			"  gl_FragColor = vec4 (((Y1) + 1.370705 * (V1)), ((Y1) - 0.337633 * (U1) - 0.698001 * (V1) ), ( (Y1) + 1.732446 * (U1)), 1.0);				 \n"
			"}                                                  \n";
	const char *fsNV12Text = 
			//precision directive has to be added if supported version is 1.0. Version 1.10 of the language does not require shaders to include this directive(in fedora it's seems it's needed)
			//but i had prob compiling the shader on some pc with default precision, read somewhere that in some systems of non-ES should not add default precision(so precision was commented in prev versions)
			//"#version 100                                   \n"
			//"#version 300 es                                   \n"
			"precision mediump float;                            \n"
			"varying vec2 v_texCoord;                            \n"
			"uniform sampler2D s_Y;                       		 \n"
			"uniform sampler2D s_U;                       		 \n"
			"uniform sampler2D s_V;                       		 \n"
			"void main()                                         \n"
			"{                                                   \n"
			"  float Y1;                                 		 \n"
			"  float U1;                                 		 \n"
			"  float V1;                                 		 \n"
			"                                                    \n"
			"  Y1 = texture2D( s_Y, v_texCoord ).r - 0.0625;  	 \n"
			"  U1 = texture2D( s_U, v_texCoord ).r - 0.5; 		 \n"
			"  V1 = texture2D( s_U, v_texCoord ).a - 0.5;		 \n"
			"  gl_FragColor = vec4 (((Y1) + 1.370705 * (V1)), ((Y1) - 0.337633 * (U1) - 0.698001 * (V1) ), ( (Y1) + 1.732446 * (U1)), 1.0);				 \n"
			"}                                                  \n";
	const char *fsRGBText = 
			//precision directive has to be added if supported version is 1.0. Version 1.10 of the language does not require shaders to include this directive(in fedora it's seems it's needed)
			//but i had prob compiling the shader on some pc with default precision, read somewhere that in some systems of non-ES should not add default precision(so precision was commented in prev versions)
			//"#version 100                                   \n"
			//"#version 300 es                                   \n"
			"precision mediump float;                            \n"
			"varying vec2 v_texCoord;                            \n"
			"uniform sampler2D s_Y;                       		 \n"
			"void main()                                         \n"
			"{                                                   \n"
			"  gl_FragColor = vec4 (texture2D( s_Y, v_texCoord ).b, texture2D( s_Y, v_texCoord ).g, texture2D( s_Y, v_texCoord ).r, texture2D( s_Y, v_texCoord ).a);				 \n"
			"}                                                  \n";

	glShaderSource(m_shader_vp, 1, &vsText, 0);
	glShaderSource(m_shader_fp, 1, &fsText, 0);
	glShaderSource(m_shaderNV12_fp, 1, &fsNV12Text, 0);
	glShaderSource(m_shaderRGB_fp, 1, &fsRGBText, 0);

	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		printf("glShaderSource failed with error: %d.\n", text);
	}

	glCompileShader(m_shader_vp);
	GLint compiled;
	glGetShaderiv ( m_shader_vp, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) 
	{
	  GLint infoLen = 0;

	  glGetShaderiv ( m_shader_vp, GL_INFO_LOG_LENGTH, &infoLen );
	  
	  if ( infoLen > 1 )
	  {
	     char* infoLog = (char*)malloc (sizeof(char) * infoLen );

	     glGetShaderInfoLog ( m_shader_vp, infoLen, NULL, infoLog );
	     printf ( "Error compiling shader:\n%s\n", infoLog );            
	     
	     free ( infoLog );
	  }

	  glDeleteShader ( m_shader_vp );
	  //return 0;
	}
	glCompileShader(m_shader_fp);
	glGetShaderiv ( m_shader_fp, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) 
	{
	  GLint infoLen = 0;

	  glGetShaderiv ( m_shader_fp, GL_INFO_LOG_LENGTH, &infoLen );
	  
	  if ( infoLen > 1 )
	  {
	     char* infoLog = (char*)malloc (sizeof(char) * infoLen );

	     glGetShaderInfoLog ( m_shader_fp, infoLen, NULL, infoLog );
	     printf ( "Error compiling shader:\n%s\n", infoLog );            
	     
	     free ( infoLog );
	  }

	  glDeleteShader ( m_shader_fp );
	  //return 0;
	}
	glCompileShader(m_shaderNV12_fp);
	glGetShaderiv ( m_shaderNV12_fp, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) 
	{
	  GLint infoLen = 0;

	  glGetShaderiv ( m_shaderNV12_fp, GL_INFO_LOG_LENGTH, &infoLen );
	  
	  if ( infoLen > 1 )
	  {
	     char* infoLog = (char*)malloc (sizeof(char) * infoLen );

	     glGetShaderInfoLog ( m_shaderNV12_fp, infoLen, NULL, infoLog );
	     printf ( "Error compiling shader:\n%s\n", infoLog );            
	     
	     free ( infoLog );
	  }

	  glDeleteShader ( m_shaderNV12_fp );
	  //return 0;
	}
	glCompileShader(m_shaderRGB_fp);
	glGetShaderiv ( m_shaderRGB_fp, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) 
	{
	  GLint infoLen = 0;

	  glGetShaderiv ( m_shaderRGB_fp, GL_INFO_LOG_LENGTH, &infoLen );
	  
	  if ( infoLen > 1 )
	  {
	     char* infoLog = (char*)malloc (sizeof(char) * infoLen );

	     glGetShaderInfoLog ( m_shaderRGB_fp, infoLen, NULL, infoLog );
	     printf ( "Error compiling shader:\n%s\n", infoLog );            
	     
	     free ( infoLog );
	  }

	  glDeleteShader ( m_shaderRGB_fp );
	  //return 0;
	}
	m_program_id = glCreateProgram();
	if(m_program_id==0)
	{
		//printf("program object value is zero.\n");
	}

	if(!glIsProgram(m_program_id))
	{
		//printf("glIsProgram returns false.\n");
	}

	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		printf("glCreateProgram failed with error: %d.\n", text);
	}
	
	m_programNV12_id = glCreateProgram();
	if(m_programNV12_id==0)
	{
		//printf("program object value is zero.\n");
	}

	if(!glIsProgram(m_programNV12_id))
	{
		//printf("glIsProgram returns false.\n");
	}

	//printf("After statement 12.\n");
	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		printf("glCreateProgram failed with error: %d.\n", text);
	}

	m_programRGB_id = glCreateProgram();
	if(m_programRGB_id==0)
	{
		//printf("program object value is zero.\n");
	}

	if(!glIsProgram(m_programRGB_id))
	{
		//printf("glIsProgram returns false.\n");
	}

	//printf("After statement 12.\n");
	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		printf("glCreateProgram failed with error: %d.\n", text);
	}
	glAttachShader(m_program_id, m_shader_fp);
	glAttachShader(m_program_id, m_shader_vp);

	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		printf("glAttachShader failed with error: %d.\n", text);
	}

	glLinkProgram(m_program_id);
	 // Check the link status
	GLint linked;
	glGetProgramiv ( m_program_id, GL_LINK_STATUS, &linked );
	if ( !linked ) 
	{
	  GLint infoLen = 0;

	  glGetProgramiv ( m_program_id, GL_INFO_LOG_LENGTH, &infoLen );
	  
	  if ( infoLen > 1 )
	  {
	     char* infoLog = (char*)malloc (sizeof(char) * infoLen );

	     glGetProgramInfoLog ( m_program_id, infoLen, NULL, infoLog );
	     printf ( "Error linking program:\n%s\n", infoLog );            
	     
	     free ( infoLog );
	  }

	  glDeleteProgram ( m_program_id );
	  //return ;
	}
	glAttachShader(m_programNV12_id, m_shaderNV12_fp);
	glAttachShader(m_programNV12_id, m_shader_vp);
	glLinkProgram(m_programNV12_id);
	glGetProgramiv ( m_programNV12_id, GL_LINK_STATUS, &linked );
	if ( !linked ) 
	{
	  GLint infoLen = 0;

	  glGetProgramiv ( m_programNV12_id, GL_INFO_LOG_LENGTH, &infoLen );
	  
	  if ( infoLen > 1 )
	  {
	     char* infoLog = (char*)malloc (sizeof(char) * infoLen );

	     glGetProgramInfoLog ( m_programNV12_id, infoLen, NULL, infoLog );
	     printf ( "Error linking program:\n%s\n", infoLog );            
	     
	     free ( infoLog );
	  }

	  glDeleteProgram ( m_programNV12_id );
	  //return ;
	}

	glAttachShader(m_programRGB_id, m_shaderRGB_fp);
	glAttachShader(m_programRGB_id, m_shader_vp);
	glLinkProgram(m_programRGB_id);
	glGetProgramiv ( m_programRGB_id, GL_LINK_STATUS, &linked );
	if ( !linked ) 
	{
	  GLint infoLen = 0;

	  glGetProgramiv ( m_programRGB_id, GL_INFO_LOG_LENGTH, &infoLen );
	  
	  if ( infoLen > 1 )
	  {
	     char* infoLog = (char*)malloc (sizeof(char) * infoLen );

	     glGetProgramInfoLog ( m_programRGB_id, infoLen, NULL, infoLog );
	     printf ( "Error linking program:\n%s\n", infoLog );            
	     
	     free ( infoLog );
	  }

	  glDeleteProgram ( m_programRGB_id );
	  //return ;
	}
	// Get the attribute locations
	m_position_loc = glGetAttribLocation ( m_program_id, "a_position" );
	m_text_coord_loc = glGetAttribLocation ( m_program_id, "a_texCoord" );
	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		printf("glGetAttribLocation failed with error: %d.\n", text);
	}
	// Get the sampler location
	m_luma_loc = glGetUniformLocation ( m_program_id, "s_Y" );
	m_chroma_b_loc = glGetUniformLocation ( m_program_id, "s_U" );
	m_chroma_r_loc = glGetUniformLocation ( m_program_id, "s_V" );
	m_positionNV12_loc = glGetAttribLocation ( m_programNV12_id, "a_position" );
	m_text_coordNV12_loc = glGetAttribLocation ( m_programNV12_id, "a_texCoord" );
	// Get the sampler location
	m_lumaNV12_loc = glGetUniformLocation ( m_programNV12_id, "s_Y" );
	m_chromaNV12_b_loc = glGetUniformLocation ( m_programNV12_id, "s_U" );

	m_positionRGB_loc = glGetAttribLocation ( m_programRGB_id, "a_position" );
	m_text_coordRGB_loc = glGetAttribLocation ( m_programRGB_id, "a_texCoord" );
	// Get the sampler location
	m_dataRGB_loc = glGetUniformLocation ( m_programRGB_id, "s_Y" );

	m_tex_w = aWidth;
	m_tex_h = aHeight;
	m_texture = new GLuint[3];
	//printf("After statement 27.\n");
	//if not used expects data supplied to glTexImage2D to aligned
	// Use tightly packed data
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
	glGenTextures(3, m_texture);   
	//printf("After statement 28.\n");
	glEnableVertexAttribArray ( m_position_loc);
	glEnableVertexAttribArray ( m_text_coord_loc);
	glUseProgram(m_program_id);
	text = glGetError();
	if(GL_NO_ERROR != text)
	{
		printf("glUseProgram failed with error: %d.\n", text);
	}
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glEnable(GL_SCISSOR_TEST);
}

/**
* setResolution: render the texture into the screen
*
* @param aWidth: width
* @param aHeight: height
*
* @return : none
*/
void setResolution(int aWidth, int aHeight)
{
	m_tex_w = aWidth;
	m_tex_h = aHeight;
}


/**
* loadImageData: load image into texture
*
* @param aLumaPtr: luma pointer
* @param aLumaSize: luma size
* @param aChromaBPtr : chroma blue pointer
* @param aChromaBSize: chroma blue size
* @param aChromaRPtr: chroma red pointer
* @param aChromaRSize: chrom red size
*
* @return : none
*/
void loadImageData(unsigned char* aLumaPtr,int aLumaSize,unsigned char* aChromaBPtr, int aChromaBSize,unsigned char* aChromaRPtr, int aChromaRSize, int format
	, int verStride	
)
{
	if(m_videoFormat != format)
	{
		m_videoFormat = format;

		if(m_videoFormat==RENDERER_YUV420)
		{
			glUseProgram(0);
			glUseProgram(m_program_id);
			glEnableVertexAttribArray ( m_position_loc);
			glEnableVertexAttribArray ( m_text_coord_loc);
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
			
			glUniform1i ( m_luma_loc, 0 );
			glUniform1i ( m_chroma_b_loc, 1 );
			glUniform1i ( m_chroma_r_loc, 2 );
		}
		else if(m_videoFormat==RENDERER_RGB)
		{
			glUseProgram(0);
			glUseProgram(m_programRGB_id);
			glEnableVertexAttribArray ( m_positionRGB_loc);
			glEnableVertexAttribArray ( m_text_coordRGB_loc);
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
			
			glUniform1i ( m_dataRGB_loc, 0 );
		}
		else
		{
			glUseProgram(0);
			glUseProgram(m_programNV12_id);
			glEnableVertexAttribArray ( m_positionNV12_loc);
			glEnableVertexAttribArray ( m_text_coordNV12_loc);
			glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
			
			glUniform1i ( m_lumaNV12_loc, 0 );
			glUniform1i ( m_chromaNV12_b_loc, 1 );
		}
	}
	int tex_height  = verStride==0?m_tex_h:verStride;
	// update the texture image:
	//glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[0]);
	if(m_videoFormat!=RENDERER_RGB)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (aLumaSize), tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, aLumaPtr);  
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (aLumaSize), tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, aLumaPtr);  
	//km:have to check if i need to do setTextureParams everytime after filling data or not and have to find a way for usability of glSubTexImage
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#if (defined(__WIN32))
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#endif
	glUniform1i ( m_videoFormat==RENDERER_YUV420?m_luma_loc:m_lumaNV12_loc, 0 );
	if(m_videoFormat!=RENDERER_RGB)
	{
	glBindTexture(GL_TEXTURE_2D, m_texture[1]);
	if(m_videoFormat!=RENDERER_YUV420)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (aChromaBSize), tex_height/2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, aChromaBPtr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (aChromaBSize), tex_height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, aChromaBPtr);  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#if (defined(__WIN32))
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#endif
		glUniform1i ( m_videoFormat==RENDERER_YUV420?m_chroma_b_loc:m_chromaNV12_b_loc, 1 );
	if(m_videoFormat!=RENDERER_NV12)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (aChromaRSize), tex_height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, aChromaRPtr);  
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	#if (defined(__WIN32))
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	#endif
			glUniform1i ( m_chroma_r_loc, 2 );
	}
	}
	glDisable(GL_TEXTURE_2D);
}

/**
* renderTexture: render the texture into the screen
*
* @param none:
*
* @return : none
*/
void renderTexture(int rotation)
{
	//glClear ( GL_COLOR_BUFFER_BIT );
	//glEnable(GL_TEXTURE_2D);
	if(m_videoFormat==RENDERER_YUV420)
	{
		// Load the vertex position
		glVertexAttribPointer ( m_position_loc, 3, GL_FLOAT, 
		GL_FALSE, 5 * sizeof(GLfloat), vVertices );
		// Load the texture coordinate
		
	}
	else if(m_videoFormat==RENDERER_RGB)
	{
		// Load the vertex position
		glVertexAttribPointer ( m_positionRGB_loc, 3, GL_FLOAT, 
		GL_FALSE, 5 * sizeof(GLfloat), vVertices );
	}
	else
	{
		// Load the vertex position
		glVertexAttribPointer ( m_positionNV12_loc, 3, GL_FLOAT, 
		GL_FALSE, 5 * sizeof(GLfloat), vVertices );
	}
	if( m_rotation!=rotation)
	{
		GLint	tex_loc = m_text_coord_loc;
		if(m_videoFormat==RENDERER_NV12)
			tex_loc = m_text_coordNV12_loc;			
		else if(m_videoFormat==RENDERER_RGB)
			tex_loc = m_text_coordRGB_loc;
		if(rotation==0 )
		{
			glVertexAttribPointer ( tex_loc, 2, GL_FLOAT,
			GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );
		}
		else if(rotation==1 )
		{
			glVertexAttribPointer ( tex_loc, 2, GL_FLOAT,
			GL_FALSE, 2 * sizeof(GLfloat), vTex90 );
		}
		else if(rotation==2 )
		{
			glVertexAttribPointer ( tex_loc, 2, GL_FLOAT,
			GL_FALSE, 2 * sizeof(GLfloat), vTex180 );
		}
		else if(rotation==3 )
		{
			glVertexAttribPointer ( tex_loc, 2, GL_FLOAT,
			GL_FALSE, 2 * sizeof(GLfloat), vTex270 );
		}
		else if(rotation==4 )
		{
			glVertexAttribPointer ( tex_loc, 2, GL_FLOAT,
			GL_FALSE, 2 * sizeof(GLfloat), vTexInverted );
		}
		m_rotation=rotation;
	}
	
	// bind the texture
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, m_texture[0]);
	if(m_videoFormat!=RENDERER_RGB)
	{
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture(GL_TEXTURE_2D, m_texture[1]);
	if(m_videoFormat!=RENDERER_NV12)
	{
		glActiveTexture ( GL_TEXTURE2 );
		glBindTexture(GL_TEXTURE_2D, m_texture[2]);
	}
	}
	glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	//glFlush();
	//glFinish ();
}


image_texture_deinit()
{
#ifdef _WIN32
	if(wglGetCurrentContext())
#endif
	{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_SCISSOR_TEST);
	
	glDisableVertexAttribArray(m_position_loc);
	glDisableVertexAttribArray(m_text_coord_loc);
	glDisableVertexAttribArray(m_positionNV12_loc);
	glDisableVertexAttribArray(m_text_coordNV12_loc);
	glDisableVertexAttribArray(m_positionRGB_loc);
	glDisableVertexAttribArray(m_text_coordRGB_loc);
	
	glUseProgram(0);
	
	glDetachShader(m_program_id, m_shader_fp);
	glDetachShader(m_program_id, m_shader_vp);
	glDetachShader(m_programNV12_id, m_shaderNV12_fp);
	glDetachShader(m_programNV12_id, m_shader_vp);
	glDetachShader(m_programRGB_id, m_shaderRGB_fp);
	glDetachShader(m_programRGB_id, m_shader_vp);
	
	glDeleteProgram(m_program_id);
	glDeleteProgram(m_programNV12_id);
	glDeleteProgram(m_programRGB_id);
	
	glDeleteShader(m_shader_fp);
	glDeleteShader(m_shaderNV12_fp);
	glDeleteShader(m_shaderRGB_fp);
	glDeleteShader(m_shader_vp);
	
	glDeleteTextures(3, m_texture);
	
	//glDeleteShader(m_program_id);
	glFlush();
	glFinish ();
	}
	if(m_texture)
		delete(m_texture);
	m_texture = 0;
	

}


