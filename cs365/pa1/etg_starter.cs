using System;
using System.IO;
using System.Collections.Generic;

/// <summary>
/// Represents a two-dimensional coordinate, with operator support for equality comparisons.
/// </summary>
struct Coords
{
    public int x;
    public int y;

    public Coords(int x, int y)
    {
        this.x = x;
        this.y = y;
    }

    public override bool Equals(Object obj)
    {
        return obj is Coords c && this == c;
    }

    public override int GetHashCode()
    {
        return this.x.GetHashCode() ^ this.y.GetHashCode();
    }

    public static bool operator ==(Coords a, Coords b)
    {
        return a.x == b.x && a.y == b.y;
    }

    public static bool operator !=(Coords a, Coords b)
    {
        return !(a == b);
    }
}

/// <summary>
/// Defines base methods for game entities to implement.
/// </summary>
abstract class Entity
{
    public virtual void look() { }
    public abstract void interact(Player player);
}

class Key : Entity
{

}

class Chest : Entity
{

}

class Skeleton : Entity
{

}

/// <summary>
/// Represents a location in the level's coordinate grid.<br/>
/// Contains a list of entities, and handles interaction with those entities.
/// </summary>
class Location
{
    public List<Entity> Entities { get; }

    /// <summary>
    /// Constructs a location, populated the list of entities.
    /// </summary>
    /// <param name="entities">The list of entities for this location. If null, a new empty list is created.</param>
    public Location(List<Entity>? entities = null)
    {
        Entities = (entities == null) ? new List<Entity>() : entities;
    }

    /// <summary>
    /// Calls look() on all entities in this Location.
    /// </summary>
    public void look()
    {
        foreach (Entity entity in Entities)
        {
            entity.look();
        }
    }

    /// <summary>
    /// Calls interact() on all entities in this Location.
    /// </summary>
    public void interact()
    {
        foreach (Entity entity in Entities)
        {
            entity.interact();
        }
    }
}

/// <summary>
/// A location where the player can exit the level.<br/>
/// Does not support any entities.
/// </summary>
class ExitLocation : Location
{
    public ExitLocation()
    {
        base();
    }

    public override void look()
    {

    }

    public override void interact()
    {

    }
}

/// <summary>
/// Contains a two-dimensional grid of locations.
/// </summary>
class Level
{
    public required Location[,] Locations { get; }

    public Level(uint width, uint height, List<Location> populatedLocations)
    {
        Locations = new Location[width, height];
        foreach (Location loc in populatedLocations)
        {
            Locations[loc.Coords.x, loc.Coords.y].Entities.Add(loc.Entity);
        }
    }
}

class Player
{
    public Coords coords { get; set; }

    public Player()
    {
        this.coords = new Coords(0, 0);
    }

    public bool is_at(Coords xy)
    {
        return this.coords == xy;
    }

    public bool is_alive() { return true; }

    public bool has_key() { return false; }

    public void print_stats()
    {
        Console.WriteLine($"  LOCATION: {this.coords.x}, {this.coords.y}");
    }
}

class Game
{
    int num_turns;
    Level level;
    public Player player { get; }

    public Game()
    {
        this.player = new Player();
    }

    public void load(string path)
    {
        this.level = new Level();

        string line;
        using (StreamReader reader = new StreamReader(path))
        {
            while ((line = reader.ReadLine()) != null)
            {
                if (line == "") { continue; }

                string[] split = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);

                if (split.Length < 3)
                {
                    Console.WriteLine($"Bad command in level file: '{line}'");
                    Environment.Exit(1);
                }

                int x = int.Parse(split[1]);
                int y = int.Parse(split[2]);
                int count = 0;

                if (split.Length > 3)
                {
                    count = int.Parse(split[3]);
                }

                switch (split[0])
                {
                    case "size":
                        // Set the level's size to x by y
                        break;
                    case "exit":
                        // Set the level's exit location to be x, y
                        break;
                    case "key":
                        // Add a key to location x, y
                        break;
                    case "loot":
                        // Add loot to location x, y with count coins
                        break;
                    case "skeleton":
                        // Add a skeleton to location x, y
                        break;
                    default:
                        Console.WriteLine($"Bad command in level file: '{line}'");
                        Environment.Exit(1);
                        break;

                }
            }
        }
    }

    public void input(string line)
    {
        this.num_turns += 1;

        // Check for exhaustion?

        Console.WriteLine("================================================================");

        string[] split = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);

        if (split.Length != 2)
        {
            Console.WriteLine($"Bad command in input: '{line}'");
            return;
        }

        Coords new_coords = this.player.coords;
        switch (split[1])
        {
            case "north":
                new_coords.y += 1;
                break;
            case "south":
                new_coords.y -= 1;
                break;
            case "east":
                new_coords.x += 1;
                break;
            case "west":
                new_coords.x -= 1;
                break;
            default:
                Console.WriteLine($"Bad command in input: '{line}'");
                return;
        }

        // Are the new coords valid?
        switch (split[0])
        {
            case "go":
                this.player.coords = new_coords;
                // Need to look at the new location and interact with it.
                break;
            case "look":
                // Need to look at the location.
                break;
            default:
                Console.WriteLine($"Bad command in input: '{line}'");
                return;
        }
    }

    bool is_over()
    {
        // What are the exit conditions?
        return false;
    }

    void print_stats()
    {
        if (this.is_over() && player.is_alive())
        {
            Console.WriteLine("You successfully escaped the graveyard!");
        }
        else
        {
            Console.WriteLine("You did not escape the graveyard. GAME OVER");
        }
        Console.WriteLine($"Game ended after {this.num_turns} turn(s).");
        player.print_stats();
    }

    public void exit()
    {
        Console.WriteLine("================================================================");
        this.print_stats();
        Environment.Exit(0);
    }

    public void exit_if_over()
    {
        if (this.is_over()) { this.exit(); }
    }

    public void intro()
    {
        Console.WriteLine("You awake in a daze to find yourself alone in the dead of night, surrounded by headstones...");
        Console.WriteLine("You must escape this graveyard.");
        Console.WriteLine("================================================================");
        // Look at the current location.
        Console.Write($"{this.player.coords.x}, {this.player.coords.y}> ");
    }
}

class ETG
{
    static void Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("ERROR: expected a single argument (the level file)");
            Environment.Exit(1);
        }

        Game game = new Game();

        game.load(args[0]);
        game.intro();

        game.exit_if_over();

        string line;

        while ((line = Console.ReadLine()) != null)
        {
            if (line == "") { continue; }
            game.input(line);
            game.exit_if_over();
            Console.Write($"{game.player.coords.x}, {game.player.coords.y}> ");
        }

        game.exit();
    }
}
