/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _IdGenerator_H_
#define _IdGenerator_H_

/**
 * @brief
 */
class IdGenerator {
  public:
    static int nextId();
    static int lastId();
    static void reset();

  private:
    static int m_lastId;
};

#endif

