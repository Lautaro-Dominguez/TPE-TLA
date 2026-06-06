package ar.edu.itba.atlyc;

import java.time.Period;
import java.util.Currency;

public class Expense extends SoftEconomicVariable{

    private String category;

    public Expense(double value, Currency currency, Period periodicity, String category, String name){
        super(value, currency, periodicity, name);
        this.category = category;
    }

    public void setCategory(String category){
        this.category = category;
    }

    public String getCategory(){
        return category;
    }

    @Override
    public String toString(){
        return "%.2f;%s;%s;%s".formatted(getValue(), getCurrency(),getPeriodicity(),getCategory());
    }
}
