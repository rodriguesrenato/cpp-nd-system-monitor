#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor
{
  public:
    float Utilization(); // TODO: See src/processor.cpp

  private:
    float totalJiffiesPrev_{0};
    float activeJiffiesPrev_{0};
};

#endif