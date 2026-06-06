package ar.edu.itba.atlyc;

import java.time.LocalDate;
import java.util.Currency;

public class Goal extends EconomicVariable{
    private LocalDate expectedDate;

    public Goal(double value, Currency currency, LocalDate expectedDate, String name){
        super(value, currency, name);
        this.expectedDate = expectedDate;
    }

    @Override
    public String toString(){
        return "%.2f;%s;%s".formatted(getValue(), getCurrency(), expectedDate);
    }
}
