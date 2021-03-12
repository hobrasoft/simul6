/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DrawingFlag_H_
#define _DrawingFlag_H_

/**
 * @brief
 */
class DrawingFlag {
    public:
        DrawingFlag(bool *f) { flag = f; *flag = true; }
       ~DrawingFlag() { *flag = false; }
    private:
        bool *flag; 
    };

#endif

