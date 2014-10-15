//Bare bones - ASSIMP LOADER
// 
// parameters:
// -filename: path to the file you want to load
// -vao: an uninitialised GLuint to use for generating a vertex attribute object
// -vbo: an uninitialised GLuint to use for generating a vertex buffer object
//
// returns:
// -the number of vertices multiplied (necessary for glDrawArrays), 0 if failed to load
//
// --example usage:
// GLuint vao,vbo,count;
// count = loadMesh("filename.dae",&vao,&vbo); //the & is vital
// 
// --drawing
// glBindVertexArray(vao);
// glDrawArrays(GL_TRIANGLES,0,count);
// glBindVertexArray(0);
//
//derived from http://nickthecoder.wordpress.com/2013/01/20/mesh-loading-with-assimp/
#include "stdafx.h"
#include <assimp\cimport.h>

GLuint loadMesh(const char *filename, GLuint *vao, GLuint *vbo)
{
	const aiScene *scene = aiImportFile(filename,aiProcessPreset_TargetRealtime_Fast);

	if(scene)
	{
		aiMesh *mesh = scene->mMeshes[0];

		unsigned int numVertices = mesh->mNumFaces*3;

		float *attributes = (float*)malloc(sizeof(float)*8*numVertices);

		GLuint count = 0;
		for(unsigned int i = 0 ; i < mesh->mNumFaces ; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			for(unsigned int j = 0 ; j < 3 ; j++)
			{
				aiVector3D vert = mesh->mVertices[face.mIndices[j]];
				aiVector3D norm = mesh->mNormals[face.mIndices[j]];
				aiVector3D uv(0.0f,0.0f,0.0f);
				if(mesh->HasTextureCoords(0))
				{
					uv = mesh->mTextureCoords[0][face.mIndices[j]];
				}
				//verts
				attributes[count] = vert.x;
				attributes[count+1] = vert.y;
				attributes[count+2] = vert.z;

				//normals
				attributes[count+3] = norm.x;
				attributes[count+4] = norm.y;
				attributes[count+5] = norm.z;

				//uvs
				attributes[count+6] = uv.x;
				attributes[count+7] = uv.y;
				count+=8;
			}
		}

		//create vao etc
		glGenVertexArrays(1,vao);
		glBindVertexArray(*vao);

		glGenBuffers(1,vbo);
		glBindBuffer(GL_ARRAY_BUFFER,*vbo);
		glBufferData(GL_ARRAY_BUFFER,count*sizeof(float),attributes,GL_STATIC_DRAW);

		//vertices => should be layout = 0 in shader
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*8,(GLvoid*)0);

		//normals => should be layout = 1 in shader
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(float)*8,(GLvoid*)(sizeof(float)*3));

		//uvs => should be layout = 2 in shader
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*8,(GLvoid*)(sizeof(float)*6));

		//done
		glBindVertexArray(0);

		free(attributes);
		aiReleaseImport(scene);
		return numVertices;
	}
	return 0;
}