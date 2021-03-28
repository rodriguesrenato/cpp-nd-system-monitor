#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
   public:
    float Utilization();

   private:
    float totalJiffiesPrev_{0.0};
    float activeJiffiesPrev_{0.0};
};

#endif