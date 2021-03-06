/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DBT_H_
#define _DBT_H_

#include <QList>

namespace Dbt {

struct StepData {
    double          time;
    int             internal_id;
    QList<double>   values_array;
    StepData() { time = 0; internal_id = 0; }
};


}

#endif

