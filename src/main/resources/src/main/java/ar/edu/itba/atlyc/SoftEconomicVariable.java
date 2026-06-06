package ar.edu.itba.atlyc;

import java.time.LocalDate;
import java.time.Period;
import java.util.Currency;

public abstract class SoftEconomicVariable extends EconomicVariable{
    private Period periodicity;

    public SoftEconomicVariable(double value, Currency currency, Period periodicity, String name){
        super(value, currency, name);
        this.periodicity = periodicity;
    }

    public void setPeriodicity(Period periodicity){
        this.periodicity = periodicity;
    }

    public Period getPeriodicity(){
        return periodicity;
    }

    public void changePeriodicity(Period newPeriodicity){
        if(newPeriodicity==null){
            throw new IllegalArgumentException();
        }

        periodicity = newPeriodicity;
    }

}
