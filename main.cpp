#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string>

struct Vertex
{
	float m_posX;
	float m_posY;
	float m_posZ;
	float m_nrmX;
	float m_nrmY;
	float m_nrmZ;
	float m_u;
	float m_v;
};

void WriteObj( char const* dstFilename, Vertex const* vertices, unsigned vertNum, uint32_t const* indices, unsigned indexNum )
{
	FILE* file = fopen( dstFilename, "w" );
	if ( !file )
	{
		printf( "Error: can't open %s\n", dstFilename );
		return;
	}


	for ( unsigned iVertex = 0; iVertex < vertNum; ++iVertex )
	{
		fprintf( file, "v %f %f %f\n", vertices[ iVertex ].m_posX, vertices[ iVertex ].m_posY, vertices[ iVertex ].m_posZ );
	}

	for ( unsigned iVertex = 0; iVertex < vertNum; ++iVertex )
	{
		fprintf( file, "vn %f %f %f\n", vertices[ iVertex ].m_nrmX, vertices[ iVertex ].m_nrmY, vertices[ iVertex ].m_nrmZ );
	}

	for ( unsigned iVertex = 0; iVertex < vertNum; ++iVertex )
	{
		fprintf( file, "vt %f %f\n", vertices[ iVertex ].m_u, vertices[ iVertex ].m_v );
	}

	for ( unsigned iFace = 0; iFace < indexNum / 3; ++iFace )
	{
		uint32_t const i0 = indices[ iFace * 3 + 0 ] + 1;
		uint32_t const i1 = indices[ iFace * 3 + 1 ] + 1;
		uint32_t const i2 = indices[ iFace * 3 + 2 ] + 1;
		fprintf( file, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", i0, i0, i0, i1, i1, i1, i2, i2, i2 );
	}

	fclose( file );
}

void Convert( char const* dstFilename, char const* srcFilename )
{
	printf( "Converting %s -> %s\n", srcFilename, dstFilename );
	FILE* file = fopen( srcFilename, "rb" );
	if ( !file )
	{
		printf( "Error: can't open %s\n", srcFilename );
		return;
	}


	uint32_t indexNum	= 0;
	uint32_t vertexNum	= 0;
	uint16_t magic		= 0;
	fread( &indexNum,	1, sizeof( indexNum ),	file );
	fread( &vertexNum,	1, sizeof( vertexNum ), file );
	fread( &magic,		1, sizeof( magic ),	file );
	if ( indexNum == 0 || vertexNum == 0 || magic != 0x120 )
	{
		printf( "Error: wrong header %s\n", srcFilename );
		fclose( file );
		return;
	}


	Vertex* vertices = new Vertex[ vertexNum ];
	fread( vertices, vertexNum, sizeof( vertices[ 0 ] ), file );

	uint32_t* indices = new uint32_t[ indexNum ];
	fread( indices, indexNum, sizeof( uint32_t ), file );

	WriteObj( dstFilename, vertices, vertexNum, indices, indexNum );

	delete[] indices;
	delete[] vertices;
	fclose( file );
}

int main( int argc, char* argv[] )
{
	printf( "Devil Daggers mesh converter\n" );
	printf( "Usage: DevilDaggersMeshToObj.exe -s srcDir -d dstDir\n" );

	char const* srcDir = "data/models";
	char const* dstDir = "converted";
	for ( int i = 0; i < argc; ++i )
	{
		if ( strcmp( argv[ i ], "-s" ) == 0 && i + 1 < argc )
		{
			srcDir = argv[ i + 1 ];
		}

		if ( strcmp( argv[ i ], "-d" ) == 0 && i + 1 < argc )
		{
			dstDir = argv[ i + 1 ];
		}
	}
	CreateDirectoryA( dstDir, nullptr );


	char searchFolder[ 1024 ];
	sprintf( searchFolder, "%s/*", srcDir );
	WIN32_FIND_DATAA fdata;
	HANDLE hFind = FindFirstFileA( searchFolder, &fdata );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( ! ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
			{
				char dstFilename[ 1024 ];
				char srcFilename[ 1024 ];
				sprintf( dstFilename, "%s/%s.obj", dstDir, fdata.cFileName );
				sprintf( srcFilename, "%s/%s", srcDir, fdata.cFileName );
				Convert( dstFilename, srcFilename );
			}
		}
		while ( FindNextFileA( hFind, &fdata ) );
	}
	else
	{
		printf( "Error: can't find source directory %s\n", searchFolder );
	}
	FindClose( hFind );


	return 0;
}
