package ar.edu.itba.atlyc;

import java.time.Period;
import java.util.Currency;

public class Income extends SoftEconomicVariable{

    public Income(double value, Currency currency, Period periodicity, String name){
        super(value, currency,periodicity, name);
    }

    @Override
    public String toString() {
        return "%.2f;%s;%s".formatted(getValue(), getCurrency(), getPeriodicity());
    }

}
