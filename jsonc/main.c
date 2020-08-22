#include <stdio.h>
#include "json_c2.c"
int main(void){
	char * str = "\
{\
	\"glossary\": {\
		\"title\": \"example glossary\",\
		\"GlossDiv\": {\
            \"title\": \"S\",\
			\"GlossList\": {\
                \"GlossEntry\": {\
                    \"ID\": \"SGML\",\
					\"SortAs\": \"SGML\",\
					\"GlossTerm\": \"Standard Generalized Markup Language\",\
					\"Acronym\": \"SGML\",\
					\"Abbrev\": \"ISO 8879:1986\",\
					\"GlossDef\": {\
                        \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\",\
						\"GlossSeeAlso\": [\"GML\", \"XML\"]\
                    },\
					\"GlossSee\": \"markup\"\
                }\
            }\
        }\
    }\
}\
";
	char * str2 = "{\"tester\":[\"1, 2, 3\", 3, 2, {\"TESTER\":true}]}";
	char * str3 =
"\
{\"web-app\": {\
  \"servlet\": [   \
    {\
      \"servlet-name\": \"cofaxCDS\",\
      \"servlet-class\": \"org.cofax.cds.CDSServlet\",\
      \"init-param\": {\
        \"configGlossary:installationAt\": \"Philadelphia, PA\",\
        \"configGlossary:adminEmail\": \"ksm@pobox.com\",\
        \"configGlossary:poweredBy\": \"Cofax\",\
        \"configGlossary:poweredByIcon\": \"/images/cofax.gif\",\
        \"configGlossary:staticPath\": \"/content/static\",\
        \"templateProcessorClass\": \"org.cofax.WysiwygTemplate\",\
        \"templateLoaderClass\": \"org.cofax.FilesTemplateLoader\",\
        \"templatePath\": \"templates\",\
        \"templateOverridePath\": \"\",\
        \"defaultListTemplate\": \"listTemplate.htm\",\
        \"defaultFileTemplate\": \"articleTemplate.htm\",\
        \"useJSP\": false,\
        \"jspListTemplate\": \"listTemplate.jsp\",\
        \"jspFileTemplate\": \"articleTemplate.jsp\",\
        \"cachePackageTagsTrack\": 200,\
        \"cachePackageTagsStore\": 200,\
        \"cachePackageTagsRefresh\": 60,\
        \"cacheTemplatesTrack\": 100,\
        \"cacheTemplatesStore\": 50,\
        \"cacheTemplatesRefresh\": 15,\
        \"cachePagesTrack\": 200,\
        \"cachePagesStore\": 100,\
        \"cachePagesRefresh\": 10,\
        \"cachePagesDirtyRead\": 10,\
        \"searchEngineListTemplate\": \"forSearchEnginesList.htm\",\
        \"searchEngineFileTemplate\": \"forSearchEngines.htm\",\
        \"searchEngineRobotsDb\": \"WEB-INF/robots.db\",\
        \"useDataStore\": true,\
        \"dataStoreClass\": \"org.cofax.SqlDataStore\",\
        \"redirectionClass\": \"org.cofax.SqlRedirection\",\
        \"dataStoreName\": \"cofax\",\
        \"dataStoreDriver\": \"com.microsoft.jdbc.sqlserver.SQLServerDriver\",\
        \"dataStoreUrl\": \"jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon\",\
        \"dataStoreUser\": \"sa\",\
        \"dataStorePassword\": \"dataStoreTestQuery\",\
        \"dataStoreTestQuery\": \"SET NOCOUNT ON;select test='test';\",\
        \"dataStoreLogFile\": \"/usr/local/tomcat/logs/datastore.log\",\
        \"dataStoreInitConns\": 10,\
        \"dataStoreMaxConns\": 100,\
        \"dataStoreConnUsageLimit\": 100,\
        \"dataStoreLogLevel\": \"debug\",\
        \"maxUrlLength\": 500}},\
    {\
      \"servlet-name\": \"cofaxEmail\",\
      \"servlet-class\": \"org.cofax.cds.EmailServlet\",\
      \"init-param\": {\
      \"mailHost\": \"mail1\",\
      \"mailHostOverride\": \"mail2\"}},\
    {\
      \"servlet-name\": \"cofaxAdmin\",\
      \"servlet-class\": \"org.cofax.cds.AdminServlet\"},\
 \
    {\
      \"servlet-name\": \"fileServlet\",\
      \"servlet-class\": \"org.cofax.cds.FileServlet\"},\
    {\
      \"servlet-name\": \"cofaxTools\",\
      \"servlet-class\": \"org.cofax.cms.CofaxToolsServlet\",\
      \"init-param\": {\
        \"templatePath\": \"toolstemplates/\",\
        \"log\": 1,\
        \"logLocation\": \"/usr/local/tomcat/logs/CofaxTools.log\",\
        \"logMaxSize\": \"\",\
        \"dataLog\": 1,\
        \"dataLogLocation\": \"/usr/local/tomcat/logs/dataLog.log\",\
        \"dataLogMaxSize\": \"\",\
        \"removePageCache\": \"/content/admin/remove?cache=pages&id=\",\
        \"removeTemplateCache\": \"/content/admin/remove?cache=templates&id=\",\
        \"fileTransferFolder\": \"/usr/local/tomcat/webapps/content/fileTransferFolder\",\
        \"lookInContext\": 1,\
        \"adminGroupID\": 4,\
        \"betaServer\": true}}],\
  \"servlet-mapping\": {\
    \"cofaxCDS\": \"/\",\
    \"cofaxEmail\": \"/cofaxutil/aemail/*\",\
    \"cofaxAdmin\": \"/admin/*\",\
    \"fileServlet\": \"/static/*\",\
    \"cofaxTools\": \"/tools/*\"},\
 \
  \"taglib\": {\
    \"taglib-uri\": \"cofax.tld\",\
    \"taglib-location\": \"/WEB-INF/tlds/cofax.tld\"}}}\
\
";
	char * str4 = 
"{\
	\"TESTER\":[\
	[1,,{\"ABC\":\"DEF\", \"Array\":[1,2,3]}] \
	[null,5,false] \
	[7,8,true]\
	]\
	\"JOY\":\"NICE\"\"Good\":\"Happy\"\
}";
	char * str5 = 
"{\
	\"string\":\"message\",\
	\"integers\":[\
		1, 10, 100000, 10000000000000000\
	],\
	\"doubles\":[\
		.1, .01, .01e+2\
		-.1, -.01, -.01e+2\
	],\
	\"booltest1\":true,\
	\"booltest2\":false,\
}";
	json_value json;
	json = json_create(str5);
	json_fprint(stdout, json);
	printf("%s\n", json_get_string(json, "string"));
	printf("%f\n", json_get_double(json, "doubles", 1));
	printf("%d\n", json_get_int(json, "integers", 1));
	printf("%d\n", json_get_bool(json, "booltest1"));
	printf("%d\n", json_get_bool(json, "booltest2"));
	json_free(json);
	
	return 0;
}
