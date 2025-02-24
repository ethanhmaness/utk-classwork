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
    public Key () { }

    public override void look()
    {
        Console.WriteLine("You see a key on the ground! Might need that to get out of here...");
    }

    public override void interact(Player player)
    {
        Console.WriteLine("You picked up a key!");
        player.hasKey = true;
    }
}

class Chest : Entity
{
    public int Coins { get; set; }

    public bool isOpen { get; set; } = false;

    public Chest(int coins = 0)
    {
        this.Coins = coins;
    }

    public override void look()
    {
        if (isOpen)
        {
            Console.WriteLine("A treasure chest sits already opened.");
        }
        else
        {
            Console.WriteLine("You see what looks like the corner of a treasure chest poking out of the ground.");
        }
    }

    public override void interact(Player player)
    {
        isOpen = true;
        if (Coins != 0)
        {
            Console.WriteLine("You open the chest and find " + Coins + " coins!");
            player.numCoins += Coins;
            Coins = 0;
        }
        else
        {
            Console.WriteLine("The chest is empty...");
        }
    }
}

class Skeleton : Entity
{
    public override void look() { }

    public override void interact(Player player)
    {
        Console.WriteLine("A bony arm juts out of the ground and grabs your ankle!");
        Console.WriteLine("You've been dragged six feet under by a skeleton.");
        player.isAlive = false;
    }
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
    public Location(List<Entity> entities = null)
    {
        Entities = entities ?? new List<Entity>();
    }

    /// <summary>
    /// Calls look() on all entities in this Location.
    /// </summary>
    public virtual void look()
    {
        List<Entity> visibleEntities = new List<Entity>();
        foreach (Entity entity in Entities)
        {
            if (!(entity is Skeleton))
            {
                visibleEntities.Add(entity);
            }
        }
        if (visibleEntities.Count == 0)
        {
            Console.WriteLine("Not much to see here.");
        }
        else
        {
            foreach (Entity entity in Entities)
            {
                entity.look();
            }
        }
    }

    /// <summary>
    /// Calls interact() on all entities in this Location.<br/>
    /// Modifies entities if necessary.
    /// </summary>
    public virtual void interact(Player player)
    {
        List<Entity> toRemove = new List<Entity>();
        foreach (Entity entity in Entities)
        {
            entity.interact(player);
            if (entity is Key)
            {
                toRemove.Add(entity);
            }
        }
        foreach (Entity entity in toRemove)
        {
            Entities.Remove(entity);
        }
    }
}

/// <summary>
/// A location where the player can exit the level.<br/>
/// Does not support any entities.
/// </summary>
class ExitLocation : Location
{
    public ExitLocation() : base() { } 

    public override void look()
    {
        Console.WriteLine("That looks like the gate out of this spooky place!");
    }

    public override void interact(Player player)
    {
        if (player.hasKey)
        {
            Console.WriteLine("You open the gate with your key!");
        }
        else
        {
            Console.WriteLine("You try to open the gate, but it's locked. Must need a key...");
        }
    }
}

/// <summary>
/// Contains a two-dimensional grid of locations.
/// </summary>
class Level
{
    public Location[,] Locations { get; set; }

    public Level() { }
}

class Player
{
    public Coords coords { get; set; }

    public bool hasKey { get; set; } = false;

    public int numCoins { get; set; } = 0;

    public bool isAlive { get; set; } = true;

    public Player()
    {
        this.coords = new Coords(0, 0);
    }

    public bool is_at(Coords xy)
    {
        return this.coords == xy;
    }

    public void print_stats()
    {
        Console.WriteLine($"  LOCATION: {this.coords.x}, {this.coords.y}");
        Console.WriteLine($"  COINS:    {numCoins}");
        Console.WriteLine($"  KEY:      {hasKey}");
        Console.WriteLine($"  DEAD:     {!isAlive}");
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
                        level.Locations = new Location[x,y];
                        for (int i = 0; i < x; i++)
                        {
                            for (int j = 0; j < y; j++)
                            {
                                level.Locations[i,j] = new Location();
                            }
                        }
                        break;
                    case "exit":
                        level.Locations[x,y] = new ExitLocation();
                        break;
                    case "key":
                        level.Locations[x,y].Entities.Add(new Key());
                        break;
                    case "loot":
                        level.Locations[x,y].Entities.Add(new Chest(count));
                        break;
                    case "skeleton":
                        level.Locations[x,y].Entities.Add(new Skeleton());
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

        if (num_turns > 2 * level.Locations.GetLength(0) * level.Locations.GetLength(1))
        {
            Console.WriteLine("You have died from exhaustion.");
            player.isAlive = false;
        }

        if (player.isAlive)
        {
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

            if (new_coords.x >= 0 && new_coords.x < level.Locations.GetLength(0) && new_coords.y >= 0 && new_coords.y < level.Locations.GetLength(1))
            {
                switch (split[0])
                {
                    case "go":
                        this.player.coords = new_coords;
                        level.Locations[player.coords.x,player.coords.y].look();
                        level.Locations[player.coords.x,player.coords.y].interact(player);
                        break;
                    case "look":
                        level.Locations[new_coords.x,new_coords.y].look();
                        break;
                    default:
                        Console.WriteLine($"Bad command in input: '{line}'");
                        return;
                }
            }
            else
            {
                Console.WriteLine("A towering wall is before you. This must be the edge of the graveyard.");
            }
        }
    }

    bool is_over()
    {
        return (!player.isAlive) || (level.Locations[player.coords.x, player.coords.y] is ExitLocation && player.hasKey);
    }

    void print_stats()
    {
        if (this.is_over() && player.isAlive)
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
        level.Locations[player.coords.x,player.coords.y].look();
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
