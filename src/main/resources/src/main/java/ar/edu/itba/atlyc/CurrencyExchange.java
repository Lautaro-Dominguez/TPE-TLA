package ar.edu.itba.atlyc;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.util.Currency;

public class CurrencyExchange {

    private static final HttpClient CLIENT = HttpClient.newHttpClient();

    public static double getExchangeRate(Currency c1, Currency c2){
        try{
            String url = "https://api.coinbase.com/v2/exchange-rates?currency=%s".formatted(c1.toString());
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(url))
                    .GET()
                    .build();

            HttpResponse<String> response =
                    CLIENT.send(
                            request,
                            HttpResponse.BodyHandlers.ofString()
                    );

            ObjectMapper mapper = new ObjectMapper();

            JsonNode root = mapper.readTree(response.body());

            String rate = root
                    .path("data")
                    .path("rates")
                    .path(c2.toString())
                    .asText();

            return new Double(rate);

        } catch (Exception e) {
            throw new RuntimeException("Error obtaining exchange rate", e);
        }
    }
}
