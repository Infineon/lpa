#ifndef WHD_OLM_INTERFACE_H
#define WHD_OLM_INTERFACE_H

#include "mbed.h"
#include "WhdSTAInterface.h"
#include "cy_lpa_wifi_olm.h"
#include "cy_lpa_wifi_ol_priv.h"

extern "C" const struct ol_desc *get_default_ol_list();

class WhdOlmInterface : public WhdSTAInterface::OlmInterface
{
public:
WhdOlmInterface(const struct ol_desc *list = ::get_default_ol_list() ) : _ols_inited(false) {
    memset(&_olm, 0, sizeof(_olm));
    cylpa_olm_init(&_olm, list);
}

virtual ~WhdOlmInterface() {
    if (_ols_inited)
    {
        cylpa_olm_deinit_ols(&_olm);
        _ols_inited = false;
    }
}

int init_ols(void *whd, void *ip) {
    int result = false;
    if (!_ols_inited)
    {
        result = cylpa_olm_init_ols(&_olm, whd, ip);
        _ols_inited = result == 0 ? true : false;
    }
    return result;
}

void deinit_ols(void) {
    if (_ols_inited)
    {
        cylpa_olm_deinit_ols(&_olm);
        _ols_inited = false;
    }
}

int sleep() {
    if (!_ols_inited)
    {
        return -1;
    }
    cylpa_olm_dispatch_pm_notification(&_olm, OL_PM_ST_GOING_TO_SLEEP);
    return 0;
}

int wake() {
    if (!_ols_inited)
    {
        return -1;
    }
    cylpa_olm_dispatch_pm_notification(&_olm, OL_PM_ST_AWAKE);
    return 0;
}

private:
bool _ols_inited;
olm_t _olm;
};

#endif /* WHD_OLM_INTERFACE_H */

