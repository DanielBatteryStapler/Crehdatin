#include "PageFooter.h"

void createPageFooter(FcgiData* fcgi, RequestData* data){
	fcgi->out << 
	"</main>"
	"</article>"
	"</body>"
	"</html>";
}
