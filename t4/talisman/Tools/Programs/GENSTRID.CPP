////////////////////////////////////////////////////////////////////////////
//
//Implementation module: GTD Gerar tabela de defini��o
//
//Generated file:        GENSTRID.cpp
//
//Module identification letters: GTD
//Module identification number:  
//
//Repository name:      Gerar e acessar tabelas de strings
//Repository file name: Z:\TALISMAN\TOOLS\BSW\TABSTR.BSW
//
//Owning organization:    
//Project:                Talisman
//List of authors
//   Id      Name
//   avs Arndt von Staa
//
//Software base change control
//    Version  Date         Authors      Description 
//    1        avs  20/11/2005  Inicio desenvolvimento
//
////////////////////////////////////////////////////////////////////////////

   #include <stdio.h>
   #include <direct.h>
   #include <string.h>
   #include <malloc.h>
   #include <process.h>

//==========================================================================
//----- Encapsulated data declarations -----
//==========================================================================

   #define DIM_ID_PARM              2
   #define DIM_BUFFER             512
   #define DIM_NOME_ARQUIVO       512
   #define DIM_NOME_STRING         34
   #define DIM_STRING             256
   #define DIM_STRINGAO          1023

   #define LIMPA_FIM                1
   #define LIMPA_INIC               2
   #define CHARS_FIM                " \r\n\t"
   #define ID_COMENTARIO            "//"

   #define SEPARADOR_EXTENSAO       '.'
   #define SEPARADOR_DIRETORIO      '\\'

   const char ExtArquivoEspec[ ]    = ".str" ;
   const char ExtArquivoTabela[ ]   = ".inc" ;

   const char CMD_OFFSET[ ]         = "<Offset>" ;

   const char idParmLinha[ ]        = "/-" ;
   const char idParmDiretorio[ ]    = "Dd" ;
   const char idParmArquivoEspec[ ] = "Ee" ;
   const char idParmTabelaDef[ ]    = "Tt" ;
   const char idParmAuxilio1[ ]     = "Hh?" ;
   const char idParmAuxilio2        = '?' ;

   #define  RES_MEM_CHAR    'm'
   #define  RES_PARM_CHAR   'p'
   #define  RES_BASE_CHAR   'b'

   const char RES_MEM[ ]            = "STR_MEM" ;
   const char RES_PARM[ ]           = "STR_PARM" ;
   const char RES_BASE[ ]           = "STR_BASE" ;

//==========================================================================
//----- Encapsulated data types -----
//==========================================================================


////////////////////////////////////////////////////////////////////////////
// 
//  Data type: GTD C�digos de retorno de fun��es
// 
////////////////////////////////////////////////////////////////////////////

   enum tpCondRet
   {

   // Fun��o executou correto

      CondRetOK ,

   // Fim de arquivo de leitura

      CondRetFimArq ,

   // Linha lida � longa demais para o buffer

      CondRetOverflow ,

   // Erro de leitura de arquivo

      CondRetErro

   }  ;


////////////////////////////////////////////////////////////////////////////
// 
//  Data type: GTD C�digos de t�rmino de processamento
// 
////////////////////////////////////////////////////////////////////////////

   enum tpCodigoErro
   {

   // Executou corretamente o programa

      CodigoOK ,

   // Foi solicitado aux�lio

      CodigoAuxilio ,

   // Arquivo n�o abre

      CodigoArquivoNaoAbre ,

   // Arquivo lista de arquivos n�o foi definido

      CodigoNaoArquivoLista ,

   // Par�metro duplicado

      CodigoParametroDuplo ,

   // Erro de processamento

      CodigoErroProcessamento ,

   // Par�metro errado

      CodigoErroParametro

   }  ;


//==========================================================================
//----- Encapsulated data items -----
//==========================================================================


// Nome do arquivo de especifica��o lido

   static char NomeArquivoEspec[ DIM_NOME_ARQUIVO ] = "" ;

// Nome do arquivo tabela de defini��o gerado

   static char NomeArquivoTabela[ DIM_NOME_ARQUIVO ] = "" ;

// Nome de c�digo do arquivo tabela de defini��o

   static char NomeArquivoCodigo[ DIM_NOME_ARQUIVO ] = "" ;

// Nome de diret�rio

   static char NomeDiretorio[ DIM_NOME_ARQUIVO ] = "" ;

// Conta especifica��es de string a gerar

   static int ContaEspecString ;

// Conta erros ao processar GERATBDF

   static int ContaErros    = 0 ;

//==========================================================================
//----- Encapsulated function prototypes -----
//==========================================================================

       
      static tpCodigoErro ProcessarParametros( int numParametros , char ** vtParametros  ) ;
       
      static void GerarTabelaDefinicao( char * NomeArquivoEspec ,
                                        char * NomeArquivoTabela ) ;
       
      static tpCondRet LerTrimmed( FILE * pArqLe    ,
                                   char * pBuffer   ,
                                   int    dimBuffer ,
                                   int    Modo       ) ;
       
      static int ObterInxExtensao( char * NomeArquivo ) ;


//==========================================================================
//----- Exported global functions -----
//==========================================================================


////////////////////////////////////////////////////////////////////////////
// 
// Function: GTD Processar linha de comando de gera tabela de definicao

   int main( int numParametros , char ** vtParametros  )
   {

      tpCodigoErro CodigoErro = CodigoOK ;

      // Identificar o programa gerador de tabela de definicao

         printf( "\nTALISMAN tool: GeraDefS - Generate access key table for the memory resident string table" ) ;
         printf( "\n     LES/DI/PUC-Rio, version 2.01, 11/dez/2014" ) ;

      // Processar parametros de geradefs

         _getcwd( NomeDiretorio , DIM_BUFFER - 1 ) ;

         CodigoErro = ProcessarParametros( numParametros , vtParametros  ) ;

         if( CodigoErro != CodigoOK )
         {
            return 4 ;
         } /* if */

      // Gerar a tabela de definicao identificada

         printf( "\n      Arquivo de strings: %s" , NomeArquivoEspec ) ;
         printf( "\n      Tabela de ids:      %s" , NomeArquivoTabela ) ;

         GerarTabelaDefinicao( NomeArquivoEspec , NomeArquivoTabela ) ;

      // Terminar GERATBDF

         if ( ContaErros > 0 )
         {
            CodigoErro = CodigoErroProcessamento ;
            printf( "\n\n>>> Foram encontrados %i erros.\n\n " ,
            ContaErros ) ;
            return 4 ;
         } /* if */

         printf( "\n\n" ) ;

         return 0 ;

   } // End of function: GTD Processar linha de comando de gera tabela de definicao


//==========================================================================
//----- Encapsulated global functions -----
//==========================================================================


////////////////////////////////////////////////////////////////////////////
// 
//  Function: GTD Processar todos os par�metros de GERATBDF
// 
////////////////////////////////////////////////////////////////////////////

   static tpCodigoErro ProcessarParametros( int numParametros , char ** vtParametros  )
   {

      char ParametroCorr[ DIM_BUFFER ] ;
      tpCodigoErro CodigoErro = CodigoOK ;

      int i ;

      int ContaDiretorio = 0 ;
      int numParmEspec   = 0 ;
      int numParmTabela  = 0 ;

      // Processar par�metros do comando GERATBDF

         ContaEspecString    = 0 ;
         ContaErros          = 0 ;

         NomeArquivoEspec[  0 ] = 0 ;
         NomeArquivoTabela[ 0 ] = 0 ;
         ParametroCorr[     0 ] = 0 ;

         for ( i = 1 ; i < numParametros ; i ++ ) {

         // Obter par�metro corrente de GERADEFS

            strcpy( ParametroCorr , vtParametros[ i ] ) ;

         // Tratar par�metro de GERADEFS

            if ( strchr( idParmLinha , ParametroCorr[ 0 ] ) != 0 )
            {

               // Processar par�metro diretorio de GERADEFS

                  if ( strchr( idParmDiretorio , ParametroCorr[ 1 ] ) != 0 )
                  {

                     if ( ContaDiretorio > 0 )
                     {
                        CodigoErro = CodigoParametroDuplo ;
                        printf( "\n>>> Diretorio especificado multiplas vezes: %s" ,
                                ParametroCorr ) ;
                        ContaErros ++ ;

                     } else
                     {
                        ContaDiretorio ++ ;
                        strcpy( NomeDiretorio , & ParametroCorr[ DIM_ID_PARM ] ) ;
                     } /* if */

                  } // end selection: Processar par�metro diretorio de GERADEFS

               // Processar par�metro arquivo especifica��o de string

                  else if ( strchr( idParmArquivoEspec , ParametroCorr[ 1 ] ) != 0 )
                  {

                     if ( numParmEspec > 0 )
                     {
                        CodigoErro = CodigoParametroDuplo ;
                        printf( "\n>>> Arquivo especifica��o %s definido multiplas vezes." , ParametroCorr ) ;
                        ContaErros ++ ;

                     } else
                     {
                        numParmEspec ++ ;
                        strcpy( NomeArquivoEspec , & ParametroCorr[ DIM_ID_PARM ] ) ;

                        if ( ObterInxExtensao( NomeArquivoEspec ) == - 1 )
                        {
                           strcat( NomeArquivoEspec , ExtArquivoEspec ) ;
                        } /* if */

                     } /* if */

                  } // end selection: Processar par�metro arquivo especifica��o de string

               // Processar par�metro tabela de defini��o a ser gerada

                  else if ( strchr( idParmTabelaDef , ParametroCorr[ 1 ] ) != 0 )
                  {

                     if ( numParmTabela > 0 )
                     {
                        CodigoErro = CodigoParametroDuplo ;
                        printf( "\n>>> Arquivo tabela %s definido multiplas vezes." , ParametroCorr ) ;
                        ContaErros ++ ;

                     } else
                     {
                        numParmTabela ++ ;
                        strcpy( NomeArquivoTabela , & ParametroCorr[ DIM_ID_PARM ] ) ;

                        if ( ObterInxExtensao( NomeArquivoTabela ) == - 1 )
                        {
                           strcat( NomeArquivoTabela , ExtArquivoTabela ) ;
                        } /* if */
                     } /* if */

                  } // end selection: Processar par�metro tabela de defini��o a ser gerada

               // Processar par�metro aux�lio de GERATBDF

                  else if ( strchr( idParmAuxilio1 , ParametroCorr[ 1 ] ) != 0 )
                  {

                     CodigoErro = CodigoAuxilio ;

                  } // end selection: Processar par�metro aux�lio de GERATBDF

               // Tratar par�metro errado para GERATBDF

                  else
                  {

                     printf( "\n>>> Parametro errado: %s" , ParametroCorr ) ;
                     CodigoErro = CodigoErroParametro ;
                     ContaErros ++ ;

                  } // end selection: Tratar par�metro errado para GERATBDF

            } // end selection: Tratar par�metro de GERADEFS

         // Processar aux�lio simples de GERADEFS

            else if ( ParametroCorr[ 0 ] == idParmAuxilio2 )
            {

               CodigoErro = CodigoAuxilio ;

            } // end selection: Processar aux�lio simples de GERADEFS

         // Tratar erro de sintaxe de GERADEFS

            else
            {

               printf( "\n>>> Sintaxe de parametro errada: %s" , ParametroCorr ) ;
               CodigoErro = CodigoErroParametro ;
               ContaErros ++ ;

            } // end selection: Tratar erro de sintaxe de GERADEFS

         } // end repetition: Processar par�metros do comando GERATBDF

      // Verificar integridade de dados de GERATBDF

         if ( _chdir( NomeDiretorio ) != 0 )
         {
            CodigoErro = CodigoNaoArquivoLista ;
            printf( "\n>>> O diretorio %s nao existe." , NomeDiretorio ) ;
            ContaErros ++ ;
            return CodigoErro ;
         } /* if */

         if ( numParmEspec == 0 )
         {
            CodigoErro = CodigoNaoArquivoLista ;
            printf( "\n>>> O arquivo especificacao de string nao foi definido." ) ;
            ContaErros ++ ;
         } /* if */

         if ( numParmTabela == 0 )
         {
            strcpy( NomeArquivoTabela, NomeArquivoEspec ) ;
            NomeArquivoTabela[ ObterInxExtensao( NomeArquivoEspec ) ] = 0 ;
            strcat( NomeArquivoTabela , ExtArquivoTabela ) ;
         } /* if */

      // Listar mensagens de aux�lo da linha de comando

         if( CodigoErro != CodigoOK )
         {

            printf( "\n\nSintaxe da linha de comando: " ) ;
            printf( "\n geradefs /E<Arquivo Espec> [ /D<Nome diretorio> ]" ) ;
            printf( "\n          [ /T<Arquivo Tabela> ] [ /h | /H | /? | ? ]" ) ;
            printf( "\n" ) ;
            printf( "\n   <Arquivo Espec> E' o nome de um arquivo que contem as" ) ;
            printf( "\n         especificacoes dos strings." ) ;
            printf( "\n   <Arquivo Tabela> parametro opcional, e' o nome do arquivo" ) ;
            printf( "\n         tabela de de definicao a ser gerado." ) ;
            printf( "\n   <Nome diretorio> e' o diretorio a partir do qual sao" ) ;
            printf( "\n         procurados os arquivos." ) ;
            printf( "\n As extensoes default sao: " ) ;
            printf( "\n   str - para os arquivos contendo especifica��es de strings." ) ;
            printf( "\n   inc - para o arquivo tabela de definicao a ser gerado." ) ;
            printf( "\n Nao sendo fornecido o nome do arquivo a ser gerado, a tabela " ) ;
            printf( "\n sera gerada no arquivo <Arquivo Espec>.inc" ) ;
            printf( "\n" ) ;

            if ( CodigoErro != CodigoAuxilio )
            {
               exit( 4 ) ;
            } /* if */

            exit ( 2 ) ;

         } // end selection: Listar mensagens de aux�lo da linha de comando

      return CodigoErro ;

   } // End of function: GTD Processar todos os par�metros de GERATBDF

////////////////////////////////////////////////////////////////////////////
// 
//  Function: GTD Gerar a tabela de defini��o
//    Gera a tabela de defini��o identificada nos par�metros
// 
////////////////////////////////////////////////////////////////////////////

   static void GerarTabelaDefinicao( char * NomeArquivoEspec ,
                                     char * NomeArquivoTabela )
   {

      FILE * pArquivoEspec  = NULL ;
      FILE * pArquivoTabela = NULL ;

      // Abrir arquivos de GERATBDF

         if ( _chdir( NomeDiretorio ) != 0 )
         {
            printf( "\n>>> O diretorio %s nao existe." , NomeDiretorio ) ;
            exit( 4 ) ;
         } /* if */

         pArquivoEspec  = fopen( NomeArquivoEspec  , "r" ) ;
         if ( pArquivoEspec == NULL )
         {
            printf( "\nNao abriu o arquivo de especificacao: %s\\%s" ,
                    NomeDiretorio , NomeArquivoEspec ) ;
            ContaErros ++ ;
         } /* if */

         pArquivoTabela = fopen( NomeArquivoTabela , "w" ) ;
         if ( pArquivoTabela == NULL )
         {
            printf( "\nNao abriu o arquivo tabela de definicao: %s\\%s" ,
                    NomeDiretorio , NomeArquivoTabela ) ;
            ContaErros ++ ;
         } /* if */

      // Gera nome de codigo

      int i ;

         strcpy( NomeArquivoCodigo , NomeArquivoTabela ) ;
         NomeArquivoCodigo[ ObterInxExtensao( NomeArquivoCodigo ) ] = 0 ;
         for( i = strlen( NomeArquivoCodigo ) - 1 ; i >= 0 ; i-- )
         {
            if ( NomeArquivoCodigo[ i ] == SEPARADOR_DIRETORIO )
            {
               strcpy( NomeArquivoCodigo , NomeArquivoCodigo + i + 1 ) ;
               break ;
            } /* if */
         } /* for */

      // Gera o texto da tabela de defini��o

         if ( ContaErros == 0 )
         {

            tpCondRet CondRet = CondRetErro ;

            char Buffer[     DIM_BUFFER ] ;
            char StringLido[ DIM_BUFFER ] ;

            long idString = -1 ;
            long Offset   = -1 ;

            int numLidos  = -1 ;

            // Gerar cabe�alho da tabela de defini��o

               fprintf( pArquivoTabela ,
                          "#ifndef _%s_str" , NomeArquivoCodigo ) ;
               fprintf( pArquivoTabela ,
                        "\n#define _%s_str" , NomeArquivoCodigo ) ;
               fprintf( pArquivoTabela ,
                        "\n" ) ;
               fprintf( pArquivoTabela ,
                        "\n// !!!!!!!!!! Generated file. Do not edit! !!!!!!!!!!" ) ;
               fprintf( pArquivoTabela ,
                        "\n" ) ;
               fprintf( pArquivoTabela ,
                        "\n///////////////////////////////////////////////////////////////////////////////" ) ;
               fprintf( pArquivoTabela ,
                        "\n//" ) ;
               fprintf( pArquivoTabela ,
                        "\n// $AT Definition table: string access keys" ) ;
               fprintf( pArquivoTabela ,
                        "\n//" ) ;
               fprintf( pArquivoTabela ,
                        "\n// Generated file:            %s" , NomeArquivoTabela ) ;
               fprintf( pArquivoTabela ,
                        "\n//" ) ;
               fprintf( pArquivoTabela ,
                        "\n// Project:                   Talisman" ) ;
               fprintf( pArquivoTabela ,
                        "\n// Owner:                     LES/DI/PUC-Rio" ) ;
               fprintf( pArquivoTabela ,
                        "\n//" ) ;
               fprintf( pArquivoTabela ,
                        "\n// String specification file: %s" , NomeArquivoEspec ) ;
               fprintf( pArquivoTabela ,
                        "\n//" ) ;
               fprintf( pArquivoTabela ,
                        "\n///////////////////////////////////////////////////////////////////////////////" ) ;
               fprintf( pArquivoTabela ,
                        "\n" ) ;
               fprintf( pArquivoTabela ,
                        "\n#include  \"CTE_STR.inc\"" ) ;
               fprintf( pArquivoTabela ,
                        "\n" ) ;

            // Obter offset das defini��es

               CondRet = LerTrimmed( pArquivoEspec , Buffer , DIM_BUFFER - 1 ,
                                     LIMPA_INIC | LIMPA_FIM ) ;

               if ( CondRet != CondRetOK )
               {
                  printf( "\nArquivo %s vazio" , NomeArquivoEspec ) ;
                  ContaErros ++ ;
               } else
               {
                  numLidos = sscanf( Buffer , " %s %li" , StringLido , &Offset ) ;
                  if ( ( numLidos != 2 )
                    || ( strcmp( StringLido , CMD_OFFSET ) != 0 ))
                  {
                     printf( "\nFaltou a defini��o de offset em %s" , NomeArquivoEspec ) ;
                     ContaErros ++ ;
                  } /* if */
               } /* if */

            // Processar as especifica��es

               if ( ContaErros == 0 )
               {

                  char Local[ DIM_BUFFER ] ;

                  CondRet = LerTrimmed( pArquivoEspec , Buffer , DIM_BUFFER - 1 ,
                                        LIMPA_INIC | LIMPA_FIM ) ;

                  while ( CondRet == CondRetOK )
                  {
                     numLidos = sscanf( Buffer , " %s %li %s" , StringLido ,
                                        &idString , Local ) ;
                     if ( numLidos != 3 )
                     {
                        printf( "\nArquivo de especificacao errado: %s" , Buffer ) ;
                        ContaErros ++ ;
                        break ;
                     } /* if */

                     fprintf( pArquivoTabela ,
                              "\n   const int %-20s = %5li" ,
                              StringLido , idString + Offset ) ;

                     switch ( Local[ 0 ] )
                     {
                        case RES_MEM_CHAR :
                              fprintf( pArquivoTabela , " | %s ;" , RES_MEM ) ;
                           break ;
                        case RES_PARM_CHAR :
                              fprintf( pArquivoTabela , " | %s ;" , RES_PARM ) ;
                           break ;
                        case RES_BASE_CHAR :
                              fprintf( pArquivoTabela , " | %s ;" , RES_BASE ) ;
                           break ;
                        default :
                           printf( "\nIdentificador de local do string errado: %s" , Buffer ) ;
                           ContaErros ++ ;
                           break ;
                     } /* switch */


                     CondRet = LerTrimmed( pArquivoEspec , Buffer , DIM_BUFFER - 1 ,
                                           LIMPA_INIC | LIMPA_FIM ) ;

                  } /* while */

               } // end selection: Processar as especifica��es

            // Gerar t�rmino do arquivo

               fprintf( pArquivoTabela ,
                        "\n" ) ;
               fprintf( pArquivoTabela ,
                        "\n#endif" ) ;
               fprintf( pArquivoTabela ,
                        "\n" ) ;
               fprintf( pArquivoTabela ,
                        "\n///////////////////////////////////////////////////////////////////////////////" ) ;
               fprintf( pArquivoTabela ,
                        "\n// End of generated definition table %s" , NomeArquivoTabela ) ;
               fprintf( pArquivoTabela ,
                        "\n///////////////////////////////////////////////////////////////////////////////" ) ;
               fprintf( pArquivoTabela ,
                        "\n" ) ;

         } // end selection: Gera o texto da tabela de defini��o

      // Fechar arquivos de GERATBDF

         if ( pArquivoEspec != NULL )
         {
            fclose( pArquivoEspec ) ;
         } /* if */

         if ( pArquivoTabela != NULL )
         {
            fclose( pArquivoTabela ) ;
         } /* if */

   } // End of function: GTD Gerar a tabela de defini��o

////////////////////////////////////////////////////////////////////////////
// 
//  Function: GTS Ler eliminando brancos no in�cio e no final
//    L� uma linha, eliminando os caracteres "branco" no in�cio e no final
//    Salta linhas de coment�rio
//    Linhas nulas ap�s elimina��o de brancos s�o saltadas
// 
// Parameters
//    pArqLe     - ponteiro para descritor de arquivo de leitura
//    pBuffer    - ponteiro para a �rea de leitura
//    dimBuffer  - dimens�o do buffer
//    Modo       - modo de limpeza, qualquer combina��o ( OR ) de 0,1 ou 2
//                 das condi��es a seguir :
//                     LIMPA_INIC - limpa antes
//                     LIMPA_FIM  - limpa no final
// 
// Return value
//    CondRetOK     - leu um buffer v�lido
//    
//    CondRetFimArq - encontrou fim de arquivo
//                    Buffer cont�m string nulo
//    
//    CondRetErro   - erro permanente de leitura
//                    Buffer cont�m string nulo
// 
////////////////////////////////////////////////////////////////////////////

   static tpCondRet LerTrimmed( FILE * pArqLe    ,
                                char * pBuffer   ,
                                int    dimBuffer ,
                                int    Modo       )
   {

      int Continua = 0 ;
      int i        = 0 ;

      // Controlar final de arquivo

         if ( feof( pArqLe ))
         {

            pBuffer[ 0 ] = 0 ;
            return CondRetFimArq ;

         } // end selection: Controlar final de arquivo

      // Procurar linha n�o vazia

      int tamLinha ;

         Continua = 1 ;

         while ( Continua ) {

         // Ler linha de arquivo

            pBuffer[ dimBuffer - 1 ] = '@' ;

            if ( fgets( pBuffer , dimBuffer , pArqLe ) == NULL )
            {
               pBuffer[ 0 ] = 0 ;
               if ( feof( pArqLe ))
               {
                  return CondRetFimArq ;
               } else {
                  return CondRetErro ;
               } /* if */
            } /* if */

            if ( pBuffer[ dimBuffer - 1 ] != '@' )
            {
               printf( "\n>>> Linha longa demais" ) ;
               ContaErros ++ ;
               return CondRetOverflow ;
            } /* if */

         // Limpar a linha

            // Eliminar lixo no final

               if ( Modo & LIMPA_FIM )
               {

                  tamLinha = ( int ) strlen( pBuffer ) ;
                  for( i = tamLinha - 1 ; i >= 0 ; i -- )
                  {
                     if ( strchr( CHARS_FIM , pBuffer[ i ] ) == NULL )
                     {
                        break ;
                     } /* if */
                  } /* for */

                  i++ ;
                  pBuffer[ i ] = 0 ;

               } // end selection: Eliminar lixo no final

            // Eliminar lixo no in�cio

               if ( Modo & LIMPA_INIC )
               {

                  tamLinha = ( int ) strlen( pBuffer ) ;
                  for( i = 0 ; i < tamLinha ; i++ )
                  {
                     if ( pBuffer[ i ] != ' ' )
                     {
                        break ;
                     } /* if */
                  } /* for */

                  if ( i > 0 )
                  {
                     strcpy( pBuffer , pBuffer + i ) ;
                  } /* if */

               } // end selection: Eliminar lixo no in�cio

         // Determinar o que fazer com a linha

            Continua = 0 ;

            if ( ( memcmp( pBuffer , ID_COMENTARIO , strlen( ID_COMENTARIO )) == 0 )
              || ( strlen( pBuffer ) == 0 ))
            {
               Continua = 1 ;
            } /* if */

         } // end repetition: Procurar linha n�o vazia

      return CondRetOK ;

   } // End of function: GTS Ler eliminando brancos no in�cio e no final

////////////////////////////////////////////////////////////////////////////
// 
//  Function: GT  Obter �ndice do nome de extens�o
//    NomeArquivo - nome do arquivo a examinar
// 
// Return value
//    Indice do caractere '.' separador do nome de extens�o
//       ser� -1 caso o nome de extens�o n�o exista
// 
////////////////////////////////////////////////////////////////////////////

   static int ObterInxExtensao( char * NomeArquivo )
   {

      int Encontrou ,
          i ;

      Encontrou = -1 ;

      for( i = strlen( NomeArquivo ) - 1 ; i > 0  ; i -- )
      {
         if ( NomeArquivo[ i ] == SEPARADOR_EXTENSAO )
         {
            Encontrou = i ;
            break ;

         } else if ( NomeArquivo[ i ] == SEPARADOR_DIRETORIO )
         {
            break ;

         } /* if */
      } /* for */

      return Encontrou ;

   } // End of function: GT  Obter �ndice do nome de extens�o

////// End of implementation module: GTD Gerar tabela de defini��o ////

