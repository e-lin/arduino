// function definitions
String URLEncode(const char * msg);

const char YQLQuery[] = "select title from feed where url='http://sudarmuthu.com/feed' limit 1";
const char Env[] = "store://datatables.org/alltableswithkeys";

void setup() {
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.println(URLEncode(YQLQuery));
  Serial.println(URLEncode(Env));
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

/**
 * URL Encode a string.
 * 
 * Based on http://www.icosaedro.it/apache/urlencode.c
 *
 */
String URLEncode(const char* msg)
{
    const char *hex = "0123456789abcdef";
    String encodedMsg = "";

    while (*msg!='\0'){
        if( ('a' <= *msg && *msg <= 'z')
                || ('A' <= *msg && *msg <= 'Z')
                || ('0' <= *msg && *msg <= '9') ) {
            encodedMsg += *msg;
        } else {
            encodedMsg += '%';
            encodedMsg += hex[*msg >> 4];
            encodedMsg += hex[*msg & 15];
        }
        msg++;
    }
    return encodedMsg;
}
