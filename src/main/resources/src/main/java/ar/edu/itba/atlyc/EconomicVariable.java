package ar.edu.itba.atlyc;

import java.time.LocalDate;
import java.util.Currency;

public abstract class EconomicVariable {
    private String name;
    private double value;
    private Currency currency;

    private LocalDate creationDate;

    public EconomicVariable(double value, Currency  currency, String name){
        this.value = value;
        this.currency = currency;
        creationDate = LocalDate.now();
        this.name = name;
    }

    public void setValue(double value){
        this.value = value;
    }

    public double getValue(){
        return value;
    }

    public void setCurrency(Currency currency){
        this.currency = currency;
    }

    public Currency getCurrency(){
        return currency;
    }

    public void setName(String name){
        this.name = name;
    }

    public String getName(){
        return name;
    }

    public LocalDate getCreationDate() {
        return creationDate;
    }

    public void exchange(Currency newCurrency){
        if(newCurrency==null){
            throw new IllegalArgumentException();
        }

        if(newCurrency.equals(this.currency)){
            return;
        }

        double rate = CurrencyExchange.getExchangeRate(this.currency, newCurrency);
        this.value = getValue() * rate;
        this.currency = newCurrency;
    }


}
