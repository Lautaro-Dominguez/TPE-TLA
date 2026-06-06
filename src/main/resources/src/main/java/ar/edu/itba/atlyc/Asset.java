package ar.edu.itba.atlyc;

import java.util.Currency;

public class Asset extends EconomicVariable {

    public Asset(double value, Currency currency, String name){
        super(value, currency, name);
    }

    @Override
    public String toString(){
        return "%.2f;%s".formatted(getValue(), getCurrency().toString());
    }
}
